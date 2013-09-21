#include "Fuji.h"

#include "MFFileSystem.h"
#include "MFHeap.h"
#include "MFStringCache.h"
#include "MFEffect_Internal.h"
#include "Asset/MFIntEffect.h"
#include "MFShader.h"

#include <string.h>

#if defined(MF_COMPILER_VISUALC)
	#pragma warning(disable: 4706)
#endif


enum MFExpressionType
{
	MFExp_Unknown,

	MFExp_Identifier,
	MFExp_Function,
	MFExp_Immediate,

	MFExp_LogicalNot,
	MFExp_Pos,
	MFExp_Neg,

	MFExp_Member,
	MFExp_LogicalAnd,
	MFExp_LogicalOr,
	MFExp_NotEquiv,
	MFExp_Equiv,
	MFExp_LessEquiv,
	MFExp_GreaterEquiv,
	MFExp_Less,
	MFExp_Greater,
	MFExp_Assignment,
	MFExp_Add,
	MFExp_Sub,
	MFExp_Mul,
	MFExp_Div,

	MFExp_UnaryOperations = MFExp_LogicalNot,
	MFExp_BinaryOperations = MFExp_Member,
};

enum MFExpressionDataType
{
	MFEDT_Unknown,

	MFEDT_Int,
	MFEDT_Float,
	MFEDT_IntVector,
	MFEDT_FloatVector,
	MFEDT_String,
	MFEDT_Bool,
	MFEDT_Code,
	MFEDT_Array,

	MFEDT_NumDataTypes
};

enum MFDataTypeProperties
{
	DTP_BoolCast = 1,
	DTP_Add = 2,
	DTP_MathOps = 4,
	DTP_LogicOps = 8,
	DTP_HasMembers = 16,
	DTP_HasData = 32,
	DTP_Compare = 64,
	DTP_Ordered = 128
};

const uint8 gDataTypeProperties[MFEDT_NumDataTypes] =
{
	0,
	DTP_BoolCast | DTP_Add | DTP_MathOps | DTP_LogicOps | DTP_Compare | DTP_Ordered,
	DTP_BoolCast | DTP_Add | DTP_MathOps | DTP_Compare | DTP_Ordered,
	DTP_Add | DTP_MathOps | DTP_LogicOps | DTP_HasMembers | DTP_HasData | DTP_Compare,
	DTP_Add | DTP_MathOps | DTP_HasMembers | DTP_HasData | DTP_Compare,
	DTP_BoolCast | DTP_Add | DTP_Compare,
	DTP_BoolCast | DTP_LogicOps | DTP_Compare,
	0,
	DTP_HasMembers | DTP_Compare
};

struct MFExpression
{
	MFExpressionType expression;
	MFExpressionDataType type;

	const char *pName;
	union
	{
		union
		{
			bool b;
			int i;
			float f;
			const char *s;
		} value;
		struct
		{
			union
			{
				int *iv;
				float *fv;
				MFExpression *a;
			};
			size_t len;
		} array;
		struct
		{
			MFExpression *pLeft, *pRight;
		} op;
	};
};

struct MFIntExpression : public MFExpression
{
	MFIntExpression()
	{
		MFZeroMemory(this, sizeof(*this));
	}

	// enough for a matrix
	char buffer[64];
};

struct MFIntEffect
{
	struct Variable
	{
		const char *pName;
		MFExpression *pValue;
	};

	struct Technique
	{
		const char *pName;
		MFExpression *pSelection;

		MFExpression *shaders[MFST_Max];
		MFArray<Variable> variables;
	};

	const char *pName;
	MFArray<Technique> techniques;
	MFArray<Variable> variables;

	MFArray<MFIntExpression> expressions;
	MFStringCache *pStringCache;
};


// *********************** .mfx parser ***********************

enum ExpressionType
{
	ET_Unknown = -1,

	ET_Statements,
		ET_Effect,
		ET_Technique,
	ET_StatementsEnd,

	ET_RangeExpression,
		ET_Scope,
		ET_Group,
		ET_Array,
	ET_RangeExpressionEnd,

	ET_EndExpression,
		ET_ScopeEnd,
		ET_GroupEnd,
		ET_ArrayEnd,
	ET_EndExpressionEnd,

	ET_UnaryExpression,
		ET_LogicalNot = ET_UnaryExpression,
		ET_Pos,
		ET_Neg,
	ET_UnaryExpressionEnd = ET_Neg,

	ET_BinaryExpression,
		ET_Member = ET_BinaryExpression,
		ET_LogicalAnd,
		ET_LogicalOr,
		ET_NotEquiv,
		ET_Equiv,
		ET_LessEquiv,
		ET_GreaterEquiv,
		ET_Less,
		ET_Greater,
		ET_Assignment,
		ET_Add,
		ET_Sub,
		ET_Mul,
		ET_Div,
	ET_BinaryExpressionEnd = ET_Div,

	ET_Terms,
		ET_Identifier,
		ET_Integer,
		ET_Float,
		ET_String,
		ET_SourceCode,
	ET_TermsEnd,

	ET_ListItem,
};

struct Symbol
{
	const char *pSymbol;
	ExpressionType type;
	uint32 len;
	uint32 precedence;
};

static const Symbol gSymbols[] =
{
	{ "(", ET_Group, 1, 8 },
	{ ")", ET_GroupEnd, 1, 8 },
	{ "{", ET_Scope, 1, 10 },
	{ "}", ET_ScopeEnd, 1, 10 },
	{ "[", ET_Array, 1, 9 },
	{ "]", ET_ArrayEnd, 1, 9 },
	{ "&&", ET_LogicalAnd, 2, 1 },
	{ "||", ET_LogicalOr, 2, 1 },
	{ "!=", ET_NotEquiv, 2, 2 },
	{ "==", ET_Equiv, 2, 2 },
	{ "<=", ET_LessEquiv, 2, 2 },
	{ ">=", ET_GreaterEquiv, 2, 2 },
	{ "<", ET_Less, 1, 2 },
	{ ">", ET_Greater, 1, 2 },
	{ "=", ET_Assignment, 1, 0 },
	{ "!", ET_LogicalNot, 1, 5 },
	{ "+", ET_Add, 1, 3 },
	{ "-", ET_Sub, 1, 3 },
	{ "*", ET_Mul, 1, 4 },
	{ "/", ET_Div, 1, 4 },
	{ ",", ET_ListItem, 1, 7 },
	{ ".", ET_Member, 1, 6 }
};

struct Token
{
	Token() : type(ET_Unknown), line(0), column(0) {}
	Token(DString token, ExpressionType type, int line, int column, int precedence = -1) : token(token), type(type), line(line), column(column), precedence(precedence) {}

	DString token;
	ExpressionType type;
	int line, column;
	int precedence;
};

struct Expression
{
	Expression()
	{
		type = ET_Unknown;
		pLeft = pRight = NULL;
		pToken = NULL;
	}

	ExpressionType type;
	Expression *pLeft, *pRight;
	Token *pToken;
};

static const char *gpConstIdentifiers[] =
{
	"true",
	"false",
	"float2",
	"float3",
	"float4",
	"float4x4",
	"int2",
	"int3",
	"int4",
	"platform",
	"numWeights",
};

struct IdentifierDesc
{
	uint8 et;
	uint8 type;
	uint8 numArgs;
	int ic;
	float fc;
	bool bc;
	float *fvc;
} gIdentifierDesc[] =
{
	{ MFExp_Immediate,	MFEDT_Bool,			0,	0, 0.f, true, NULL },
	{ MFExp_Immediate,	MFEDT_Bool,			0,	0, 0.f, false, NULL },
	{ MFExp_Function,	MFEDT_FloatVector,	2,	0, 0.f, false, NULL },
	{ MFExp_Function,	MFEDT_FloatVector,	3,	0, 0.f, false, NULL },
	{ MFExp_Function,	MFEDT_FloatVector,	4,	0, 0.f, false, NULL },
	{ MFExp_Function,	MFEDT_FloatVector,	16,	0, 0.f, false, NULL },
	{ MFExp_Function,	MFEDT_IntVector,	2,	0, 0.f, false, NULL },
	{ MFExp_Function,	MFEDT_IntVector,	3,	0, 0.f, false, NULL },
	{ MFExp_Function,	MFEDT_IntVector,	4,	0, 0.f, false, NULL },
	{ MFExp_Identifier,	MFEDT_String,		0,	0, 0.f, false, NULL },
	{ MFExp_Identifier,	MFEDT_Int,			0,	0, 0.f, false, NULL },
};


// **** fuctions ****

Expression *GetExpression(int precedence, DSlice<Token> &tokens);

static MFIntEffect *Error(const char *pMessage, const char *pFilename, int line, int col, char *pBuffer)
{
	const char *pFilePath = MFFileSystem_ResolveSystemPath(pFilename);
	MFDebug_Log(1, MFStr("%s(%d,%d): %s", pFilePath, line, col, pMessage));
	MFHeap_Free(pBuffer);
	return NULL;
}

Token* Expect(DSlice<Token> &tokens, ExpressionType type)
{
	Token *pT = &tokens[0];
	if(tokens.length == 0 || tokens[0].type != type)
		pT = NULL;
	tokens = tokens.popFront();
	return pT;
}

Expression *GetScope(DSlice<Token> &tokens)
{
	// a scope is a sequence of expressions...
	Expect(tokens, ET_Scope);

	Expression *scope = NULL;
	Expression *prev = NULL;
	while(tokens.length && tokens[0].type != ET_ScopeEnd)
	{
		Expression *s = new Expression();
		s->type = ET_Scope;
		s->pLeft = GetExpression(0, tokens);

		if(!scope)
			scope = s;
		if(prev)
			prev->pRight = s;
		prev = s;
	}

	Expect(tokens, ET_ScopeEnd);
	return scope;
}

Expression *GetTerm(DSlice<Token> &tokens)
{
	if(tokens.length == 0)
		return NULL;

	Token &next = tokens[0];
	tokens = tokens.popFront();

	if(next.type == ET_Identifier && next.token == "effect")
	{
		Token *pIdentifier = Expect(tokens, ET_Identifier);

		Expression *e = new Expression();
		e->pToken = pIdentifier;
		e->type = ET_Effect;
		e->pRight = GetScope(tokens);
		return e;
	}
	else if(next.type == ET_Identifier && next.token == "technique")
	{
		Token *pIdentifier = NULL;
		if(tokens.length > 0 && tokens[0].type == ET_String)
		{
			pIdentifier = &tokens[0];
			tokens = tokens.popFront();
		}

		Expression *c = NULL;
		if(tokens.length > 0 && tokens[0].type == ET_Group)
		{
			tokens = tokens.popFront();
			c = GetExpression(0, tokens);
			Expect(tokens, ET_GroupEnd);
		}

		Expression *e = new Expression();
		e->pToken = pIdentifier;
		e->type = ET_Technique;
		e->pLeft = c;
		e->pRight = GetScope(tokens);
		return e;
	}
	else if(next.type >= ET_UnaryExpression && next.type <= ET_UnaryExpressionEnd)
	{
		Expression *e = new Expression();
		e->pToken = &next;
		e->type = next.type;
		e->pRight = GetExpression(next.precedence, tokens);
		return e;
	}
	else if(next.type == ET_Group)
	{
		Expression *e = GetExpression(0, tokens);
		Expect(tokens, ET_GroupEnd);
		return e;
	}
	else if(next.type > ET_Terms && next.type < ET_TermsEnd)
	{
		Expression *e = new Expression();
		e->pToken = &next;
		e->type = next.type;

		if(next.type == ET_Identifier)
		{
			if(tokens.length > 0 && tokens[0].type == ET_Group)
			{
				tokens = tokens.popFront();

				if(tokens.length > 0 && tokens[0].type != ET_GroupEnd)
				{
					Expression *prev = e;
					while(true)
					{
						Expression *a = new Expression();
						a->type = ET_ListItem;
						a->pLeft = GetExpression(0, tokens);

						prev->pRight = a;
						prev = a;

						if(tokens.length > 0 && tokens[0].type == ET_ListItem)
							tokens = tokens.popFront();
						else
							break;
					}
				}
				Expect(tokens, ET_GroupEnd);
			}

			if(next.token == "src")
			{
				Token *pCode = Expect(tokens, ET_SourceCode);
				e->pToken = pCode;
				e->type = ET_SourceCode;
			}
		}

		return e;
	}

	Error("Error!", "", next.line, next.column, NULL);
	return NULL;
}

Expression *GetExpression(int precedence, DSlice<Token> &tokens)
{
	Expression *e = GetTerm(tokens);

	if(tokens.length == 0)
		return e;

	Token *next = &tokens[0];
	while((next->type >= ET_BinaryExpression && next->type <= ET_BinaryExpressionEnd) && next->precedence >= precedence)
	{
		tokens = tokens.popFront();
/*
		const q := case associativity(op)
					of Right: prec( op )
						Left:  1+prec( op )
*/
		bool bLeftAssoc = true;
		Expression *e2 = GetExpression(next->precedence + (bLeftAssoc ? 1 : 0), tokens);

		Expression *op = new Expression();
		op->pToken = next;
		op->type = next->type;
		op->pLeft = e;
		op->pRight = e2;
		e = op;

		if(tokens.length == 0)
			break;
		next = &tokens[0];
	}

	return e;
}

size_t CountExpressions(Expression *e)
{
	size_t num = 1;
	if(e->pLeft)
		num += CountExpressions(e->pLeft);
	if(e->pRight)
		num += CountExpressions(e->pRight);
	return num;
}

void FreeExpressionTree(Expression *e)
{
	if(e->pLeft)
		FreeExpressionTree(e->pLeft);
	if(e->pRight)
		FreeExpressionTree(e->pRight);
	delete e;
}

MFIntExpression *CopyTree(Expression *pExp, MFIntEffect *pEffect, MFIntEffect::Technique *pTechnique, MFIntExpression *pE = NULL)
{
	MFIntExpression &e = pE ? *pE : pEffect->expressions.push();

	if(pExp->type >= ET_UnaryExpression && pExp->type <= ET_BinaryExpressionEnd)
	{
		e.expression = (MFExpressionType)(MFExp_UnaryOperations + (pExp->type - ET_UnaryExpression));

		// copy the sub-expressions
		MFIntExpression *l = NULL, *r;
		MFExpressionDataType lt = MFEDT_NumDataTypes, rt;
		if(pExp->type >= ET_BinaryExpression)
		{
			l = CopyTree(pExp->pLeft, pEffect, pTechnique);
			lt = l->type;
		}
		r = CopyTree(pExp->pRight, pEffect, pTechnique);
		rt = r->type;

		e.op.pLeft = l;
		e.op.pRight = r;

		// messy work to find the result type of the expression
		switch(e.expression)
		{
			case MFExp_LogicalAnd:
			case MFExp_LogicalOr:
				MFDebug_Assert(gDataTypeProperties[lt] & DTP_BoolCast, "Error!");
			case MFExp_LogicalNot:
				MFDebug_Assert(gDataTypeProperties[rt] & DTP_BoolCast, "Error!");
				e.type = MFEDT_Bool;
				break;
			case MFExp_Pos:
			case MFExp_Neg:
				MFDebug_Assert(gDataTypeProperties[rt] & DTP_MathOps, "Error!");
				e.type = rt;
				break;
			case MFExp_Member:
				MFDebug_Assert(gDataTypeProperties[lt] & DTP_HasMembers, "Error!");
				MFDebug_Assert(r->expression == MFExp_Identifier, "Expected: Member identifier.");
				if(!MFString_CaseCmp(r->pName, "length"))
					e.type = MFEDT_Int;
				else if(l->type == MFEDT_IntVector)
					e.type = MFEDT_Int;
				else if(l->type == MFEDT_FloatVector)
					e.type = MFEDT_Float;
				break;
			case MFExp_LessEquiv:
			case MFExp_GreaterEquiv:
			case MFExp_Less:
			case MFExp_Greater:
				MFDebug_Assert((gDataTypeProperties[lt] & DTP_Ordered) && (gDataTypeProperties[rt] & DTP_Ordered), "Error!");
			case MFExp_NotEquiv:
			case MFExp_Equiv:
				MFDebug_Assert((gDataTypeProperties[lt] & DTP_Compare) && (gDataTypeProperties[rt] & DTP_Compare), "Error!");
				MFDebug_Assert(lt == rt || (lt < MFEDT_String && rt < MFEDT_String), "Error!");
				e.type = MFEDT_Bool;
				break;
			case MFExp_Assignment:
				e.type = r->type;
				break;
			case MFExp_Sub:
			case MFExp_Mul:
				MFDebug_Assert((gDataTypeProperties[lt] & DTP_MathOps) && (gDataTypeProperties[rt] & DTP_MathOps), "Error!");
			case MFExp_Add:
				MFDebug_Assert((gDataTypeProperties[lt] & DTP_Add) && (gDataTypeProperties[rt] & DTP_Add), "Error!");
				MFDebug_Assert(lt == rt || (lt < MFEDT_String && rt < MFEDT_String), "Error!");
				e.type = rt;
				if(lt != rt)
					e.type = (MFExpressionDataType)(lt | rt);
				break;
			case MFExp_Div:
				MFDebug_Assert((gDataTypeProperties[lt] & DTP_MathOps) && (gDataTypeProperties[rt] & DTP_MathOps), "Error!");
				e.type = (MFExpressionDataType)(lt | rt | MFEDT_Float);
				break;
		}
	}
	else
	{
		switch(pExp->type)
		{
			case ET_Identifier:
			{
				e.pName = MFStringCache_AddN(pEffect->pStringCache, pExp->pToken->token.ptr, pExp->pToken->token.length);

				int c = MFString_Enumerate(MFStrN(pExp->pToken->token.ptr, pExp->pToken->token.length), gpConstIdentifiers, sizeof(gpConstIdentifiers) / sizeof(gpConstIdentifiers[0]), true);
				if(c >= 0)
				{
					e.expression = (MFExpressionType)gIdentifierDesc[c].et;
					e.type = (MFExpressionDataType)gIdentifierDesc[c].type;
				}
				else
				{
					bool bFound = false;
					for(int i=0; i<MFSCB_Max; ++i)
					{
						const char *pName = MFStateBlock_GetRenderStateName(MFSB_CT_Bool, i);
						if(pExp->pToken->token == pName)
						{
							e.expression = MFExp_Identifier;
							e.type = MFEDT_Bool;
							bFound = true;
							break;
						}
					}

					if(!bFound && pTechnique)
					{
						for(size_t i=0; i<pTechnique->variables.size(); ++i)
						{
							if(pExp->pToken->token == pTechnique->variables[i].pName)
							{
								e.expression = MFExp_Identifier;
								e.type = pTechnique->variables[i].pValue->type;
								bFound = true;
								break;
							}
						}
					}

					if(!bFound)
					{
						for(size_t i=0; i<pEffect->variables.size(); ++i)
						{
							if(pExp->pToken->token == pEffect->variables[i].pName)
							{
								e.expression = MFExp_Identifier;
								e.type = pEffect->variables[i].pValue->type;
								bFound = true;
								break;
							}
						}
					}

					if(!bFound)
						MFDebug_Assert(false, "Unknown identifier!");
				}

				if(e.expression == MFExp_Function)
				{
					e.array.len = gIdentifierDesc[c].numArgs;
					e.array.a = pEffect->expressions.getPointer();
					pEffect->expressions.resize(pEffect->expressions.size() + e.array.len);
					int i = 0;
					Expression *pE = pExp->pRight;
					while(pE)
					{
						CopyTree(pE->pLeft, pEffect, pTechnique, (MFIntExpression*)e.array.a + i++);
						pE = pE->pRight;
					}
				}
				else if(e.expression == MFExp_Immediate)
				{
					switch(e.type)
					{
						case MFEDT_Bool:
							e.value.b = gIdentifierDesc[c].bc;
							break;
						default:
							MFDebug_Assert(false, "!");
							break;
					}
				}
				break;
			}
			case ET_Integer:
				e.expression = MFExp_Immediate; e.type = MFEDT_Int;
				e.value.i = MFString_AsciiToInteger(MFStr(pExp->pToken->token.ptr, pExp->pToken->token.length));
				break;
			case ET_Float:
				e.expression = MFExp_Immediate; e.type = MFEDT_Float;
				e.value.f = MFString_AsciiToFloat(MFStr(pExp->pToken->token.ptr, pExp->pToken->token.length));
				break;
			case ET_String:
			case ET_SourceCode:
				e.expression = MFExp_Immediate;
				e.type = pExp->type == ET_String ? MFEDT_String : MFEDT_Code;
				e.value.s = MFStringCache_AddN(pEffect->pStringCache, pExp->pToken->token.ptr, pExp->pToken->token.length);
				break;
		}
	}

	return &e;
}

// ********************************************************************

MF_API MFIntEffect *MFIntEffect_CreateFromSourceData(const char *pFilename)
{
	size_t bytes = 0;
	char *pBuffer = MFFileSystem_Load(pFilename, &bytes);

	if(!pBuffer)
		return NULL;

	// tokenise the effect file
	MFArray<Token> tokens;
	size_t line = 0;
	char *pLineStart = pBuffer;

	DString buffer(pBuffer, bytes);
	bool bExpectCode = false;
	while(buffer.length)
	{
		char c;
		while((c = buffer[0]) == ' ' || c == '\t' || c == '\r')
		{
			buffer = buffer.popFront();
			c = buffer.length ? buffer[0] : 0;
		}

		if(c == '\n')
		{
			++line;
			buffer = buffer.popFront();
			pLineStart = buffer.ptr;
		}
		else if(MFIsNumeric(c))
		{
			// it's	a number
			int col = (int)(buffer.ptr - pLineStart);

			size_t len = 1;
			ExpressionType et = ET_Integer;

			bool bNoDot = true;
			bool bIsHex = false;
			if(MFToLower(buffer[1]) == 'x')
			{
				bIsHex = true;
				len = 2;
			}

			for(; len<buffer.length; ++len)
			{
				c = buffer[len];
				if(MFIsNumeric(c) || (bIsHex && MFIsHex(c)))
					continue;
				else if(c == '.' && !bIsHex && bNoDot)
				{
					bNoDot = false;
					et = ET_Float;
					continue;
				}
				else if(c == '.' || c == '_' || MFIsAlpha(c))
					return Error("error: Invalid number.", pFilename, line, col, pBuffer);
				break;
			}

			tokens.push(Token(MFString(buffer.ptr, len), et, line, col));
			buffer = buffer.popFront(len);
		}
		else if(MFIsAlpha(c) || c == '_')
		{
			// it's an identifier
			int col = (int)(buffer.ptr - pLineStart);

			size_t len = 1;

			for(; len<buffer.length; ++len)
			{
				c = buffer[len];
				if(MFIsAlphaNumeric(c) || c == '_')
					continue;
				break;
			}

			Token &t = tokens.push(Token(MFString(buffer.ptr, len), ET_Identifier, line, col));
			buffer = buffer.popFront(len);

			if(t.token == "src")
				bExpectCode = true;
		}
		else if(c == '"')
		{
			// it's a string
			int col = (int)(buffer.ptr - pLineStart);

			buffer = buffer.popFront();

			size_t len = 0;
			for(; len<buffer.length && buffer[len] != '"'; ++len)
			{
				if(buffer[len] == '\n')
					return Error("error: String not terminated.", pFilename, line, col, pBuffer);
			}

			tokens.push(Token(MFString(buffer.ptr, len), ET_String, line, col));
			buffer = buffer.popFront(len);

			if(buffer.length == 0)
				return Error("error: String not terminated.", pFilename, line, col, pBuffer);
			buffer = buffer.popFront();
		}
		else
		{
			// it's some sort of symbol
			int col = (int)(buffer.ptr - pLineStart);

			if(bExpectCode && buffer[0] == '{')
			{
				bExpectCode = false;

				buffer = buffer.popFront();

				// read until the next matching '}' as a string
				int depth = 0;
				size_t len = 0;
				bool bInString = false;
				for(; len<buffer.length && (buffer[len] != '}' || depth > 0); ++len)
				{
					if(buffer[len] == '"')
						bInString = !bInString;
					else if(buffer[len] == '\n')
						++line;
					else if(!bInString && buffer[len] == '{')
						++depth;
					else if(!bInString && buffer[len] == '}')
						--depth;
				}

				tokens.push(Token(MFString(buffer.ptr, len), ET_SourceCode, line, col));
				buffer = buffer.popFront(len);

				if(buffer.length == 0)
					return Error("error: Mismatching braces?", pFilename, line, col, pBuffer);
				buffer = buffer.popFront();
			}
			else
			{
				bool bFound = false;
				for(int a=0; a<sizeof(gSymbols)/sizeof(gSymbols[0]); ++a)
				{
					if(gSymbols[a].len <= buffer.length && !MFString_CompareN(buffer.ptr, gSymbols[a].pSymbol, gSymbols[a].len))
					{
						tokens.push(Token(buffer.slice(0, gSymbols[a].len), gSymbols[a].type, line, col, gSymbols[a].precedence));
						buffer = buffer.popFront(gSymbols[a].len);
						bFound = true;
						break;
					}
				}

				if(!bFound)
					return Error(MFStr("error: Unexpected symbol '%c'", buffer[0]), pFilename, line, col, pBuffer);
			}
		}
	}

	// parse the effect
	DSlice<Token> tok(tokens.getPointer(), tokens.size());
	Expression *pRoot = GetExpression(0, tok);

	MFIntEffect *pEffect = NULL;
	if(pRoot->type == ET_Effect)
	{
		// build the MFEffect structure
		pEffect = new MFIntEffect();
		pEffect->expressions.reserve(CountExpressions(pRoot));
		pEffect->pStringCache = MFStringCache_Create(bytes);

		pEffect->pName = pRoot->pToken ? MFStringCache_AddN(pEffect->pStringCache, pRoot->pToken->token.ptr, pRoot->pToken->token.length) : NULL;

		Expression *pEffectScope = pRoot->pRight;
		while(pEffectScope)
		{
			Expression *pStatement = pEffectScope->pLeft;

			if(pStatement->type == ET_Technique)
			{
				MFIntEffect::Technique &t = pEffect->techniques.push();
				MFZeroMemory(&t, sizeof(t));

				t.pName = pStatement->pToken ? MFStringCache_AddN(pEffect->pStringCache, pStatement->pToken->token.ptr, pStatement->pToken->token.length) : NULL;
				t.pSelection = pStatement->pLeft ? CopyTree(pStatement->pLeft, pEffect, NULL) : NULL;

				Expression *pTechniqueScope = pStatement->pRight;
				while(pTechniqueScope)
				{
					Expression *pS = pTechniqueScope->pLeft;

					if(pS->type == ET_Assignment)
					{
						static const char *gpShaders[] =
						{
							"VertexShader",
							"PixelShader",
							"GeometryShader",
							"DomainShader",
							"HullShader",
							"ComputeShader"
						};

						MFDebug_Assert(pS->pLeft->type == ET_Identifier, "Left of assignment must be identifier");
						int shaderType = MFString_Enumerate(MFStrN(pS->pLeft->pToken->token.ptr, pS->pLeft->pToken->token.length), gpShaders, sizeof(gpShaders) / sizeof(gpShaders[0]), true);
						if(shaderType > -1)
						{
							t.shaders[shaderType] = CopyTree(pS->pRight, pEffect, &t);
						}
						else
						{
							MFIntEffect::Variable v;
							v.pName = MFStringCache_AddN(pEffect->pStringCache, pS->pLeft->pToken->token.ptr, pS->pLeft->pToken->token.length);
							v.pValue = CopyTree(pS->pRight, pEffect, &t);
							t.variables.push(v);
						}
					}

					pTechniqueScope = pTechniqueScope->pRight;
				}
			}
			else if(pStatement->type == ET_Assignment)
			{
				// we're assigning a variable
				MFDebug_Assert(pStatement->pLeft->type == ET_Identifier, "Left of assignment must be identifier");

				MFIntEffect::Variable v;
				v.pName = MFStringCache_AddN(pEffect->pStringCache, pStatement->pLeft->pToken->token.ptr, pStatement->pLeft->pToken->token.length);
				v.pValue = CopyTree(pStatement->pRight, pEffect, NULL);
				pEffect->variables.push(v);
			}

			pEffectScope = pEffectScope->pRight;
		}
	}

	FreeExpressionTree(pRoot);
	MFHeap_Free(pBuffer);

	return pEffect;
}

MF_API void MFIntEffect_Destroy(MFIntEffect *pEffect)
{
	MFStringCache_Destroy(pEffect->pStringCache);
	delete pEffect;
}

MF_API void MFIntEffect_CreateRuntimeData(MFIntEffect *pEffect, MFEffect **ppOutputEffect, size_t *pSize, MFPlatform platform, size_t extraBytes)
{
	// flatten constants

	// copy into final structure...
}
