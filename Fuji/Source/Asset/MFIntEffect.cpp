#include "Fuji_Internal.h"
#include "MFFileSystem.h"
#include "MFHeap.h"
#include "MFStringCache.h"
#include "MFEffect_Internal.h"
#include "MFShader_Internal.h"
#include "MFShader.h"
#include "Util.h"
#include "Asset/MFIntEffect.h"

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

struct MFIntExpression
{
	MFIntExpression()
	{
		MFZeroMemory(this, sizeof(*this));
	}

	MFExpressionType expression;
	MFExpressionDataType type;
	int line, column;

	const char *pName;
	union
	{
		struct
		{
			bool b;
			int i;
			float f;
			const char *s;
			struct
			{
				const char *pCode;
				const char *pLanguage;
			} code;
		} value;
		struct
		{
			union
			{
				int iv[16];
				float fv[16];
				MFIntExpression *a;
			};
			size_t len;
		} array;
		struct
		{
			MFIntExpression *pLeft, *pRight;
		} op;
	};
};

struct MFIntEffect
{
	struct Variable
	{
		const char *pName;
		MFIntExpression *pValue;

		MFIntExpression *pSimplified; // temporary
	};

	struct Technique
	{
		const char *pName;
		MFIntExpression *pSelection;

		MFIntExpression *shaders[MFST_Max];
		MFArray<Variable> variables;

		// temporary...
		MFIntExpression *pSimplified;
		MFIntExpression *simplifiedShaders[MFST_Max];
		bool bIncluded;
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

static void Error(const char *pMessage, const char *pFilename, int line, int col, char *pBuffer)
{
	const char *pFilePath = MFFileSystem_ResolveSystemPath(pFilename);
	MFDebug_Log(1, MFStr("%s(%d,%d): %s", pFilePath, line, col, pMessage));
	MFHeap_Free(pBuffer);
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

	if(next.type == ET_Identifier && next.token.eq("effect"))
	{
		Token *pIdentifier = Expect(tokens, ET_Identifier);

		Expression *e = new Expression();
		e->pToken = pIdentifier;
		e->type = ET_Effect;
		e->pRight = GetScope(tokens);
		return e;
	}
	else if(next.type == ET_Identifier && next.token.eq("technique"))
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

			if(next.token.eq("src"))
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

MFIntExpression *CopyTree(Expression *pExp, MFIntEffect *pEffect, MFIntEffect::Technique *pTechnique, MFIntExpression *_pE = NULL)
{
	MFIntExpression &e = _pE ? *_pE : pEffect->expressions.push();
	e.line = pExp->pToken->line;
	e.column = pExp->pToken->column;

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
			default:
				MFUNREACHABLE;
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
						if(pExp->pToken->token.eq(pName))
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
							if(pExp->pToken->token.eq(pTechnique->variables[i].pName))
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
							if(pExp->pToken->token.eq(pEffect->variables[i].pName))
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
					size_t numExpresions = pEffect->expressions.size();
					e.array.a = pEffect->expressions.getPointer() + numExpresions;
					e.array.len = gIdentifierDesc[c].numArgs;
					pEffect->expressions.resize(numExpresions + e.array.len);
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
				e.expression = MFExp_Immediate;
				e.type = MFEDT_String;
				e.value.s = MFStringCache_AddN(pEffect->pStringCache, pExp->pToken->token.ptr, pExp->pToken->token.length);
			case ET_SourceCode:
				e.expression = MFExp_Immediate;
				e.type = MFEDT_Code;
				e.value.code.pCode = MFStringCache_AddN(pEffect->pStringCache, pExp->pToken->token.ptr, pExp->pToken->token.length);
				e.value.code.pLanguage = pExp->pRight ? MFStringCache_AddN(pEffect->pStringCache, pExp->pRight->pLeft->pToken->token.ptr, pExp->pRight->pLeft->pToken->token.length) : NULL;
				break;
			default:
				MFUNREACHABLE;
				break;
		}
	}

	return &e;
}

// ********************************************************************

static char *Lex(const char *pFilename, MFArray<Token> &tokens, size_t *pBytes)
{
	size_t bytes = 0;
	char *pBuffer = MFFileSystem_Load(pFilename, &bytes);

	if(!pBuffer)
		return NULL;
	if(pBytes)
		*pBytes = bytes;

	int line = 1;
	const char *pLineStart = pBuffer;

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
				{
					Error("error: Invalid number.", pFilename, line, col, pBuffer);
					return NULL;
				}
				break;
			}

			tokens.push(Token(buffer.slice(0, len), et, line, col));
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

			Token &t = tokens.push(Token(buffer.slice(0, len), ET_Identifier, line, col));
			buffer = buffer.popFront(len);

			if(t.token.eq("src"))
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
				{
					Error("error: String not terminated.", pFilename, line, col, pBuffer);
					return NULL;
				}
			}

			tokens.push(Token(buffer.slice(0, len), ET_String, line, col));
			buffer = buffer.popFront(len);

			if(buffer.length == 0)
			{
				Error("error: String not terminated.", pFilename, line, col, pBuffer);
				return NULL;
			}
			buffer = buffer.popFront();
		}
		else
		{
			// it's some sort of symbol
			int col = (int)(buffer.ptr - pLineStart);

			if(bExpectCode && buffer[0] == '{')
			{
				bExpectCode = false;
				int startLine = line;

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

				tokens.push(Token(buffer.slice(0, len), ET_SourceCode, startLine, col));
				buffer = buffer.popFront(len);

				if(buffer.length == 0)
				{
					Error("error: Mismatching braces?", pFilename, line, col, pBuffer);
					return NULL;
				}
				buffer = buffer.popFront();
			}
			else
			{
				bool bFound = false;
				for(size_t a=0; a<sizeof(gSymbols)/sizeof(gSymbols[0]); ++a)
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
				{
					Error(MFStr("error: Unexpected symbol '%c'", buffer[0]), pFilename, line, col, pBuffer);
					return NULL;
				}
			}
		}
	}

	return pBuffer;
}

MF_API MFIntEffect *MFIntEffect_CreateFromSourceData(const char *pFilename)
{
	// tokenise the effect file
	MFArray<Token> tokens;
	size_t bytes;
	char *pBuffer = Lex(pFilename, tokens, &bytes);

	if(!pBuffer)
		return NULL;

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

static const char *pPlatforms[FP_Max] =
{
	"windows",
	"xbox",
	"linux",
	"psp",
	"ps2",
	"dc",
	"gc",
	"osx",
	"amiga",
	"x360",
	"ps3",
	"wii",
	"symbian",
	"ios",
	"android",
	"windowsmobile",
	"nacl",
	"web"
};

MFIntExpression SimplifyExpression(MFIntExpression &exp, MFIntEffect &effect, MFIntEffect::Technique *pTechnique, MFArray<MFIntExpression> &expressions, MFPlatform platform)
{
	MFIntExpression e;

	switch(exp.expression)
	{
		case MFExp_Immediate:
			return exp;
		case MFExp_Identifier:
			if(!MFString_Compare(exp.pName, "platform"))
			{
				e.expression = MFExp_Immediate;
				e.type = MFEDT_String;
				e.value.s = pPlatforms[platform];
			}
			else
			{
				for(size_t i=0; i<effect.variables.size(); ++i)
				{
					if(!MFString_Compare(exp.pName, effect.variables[i].pName))
						return *effect.variables[i].pSimplified;
				}
				if(pTechnique)
				{
					for(size_t i=0; i<pTechnique->variables.size(); ++i)
					{
						if(!MFString_Compare(exp.pName, pTechnique->variables[i].pName))
							return *pTechnique->variables[i].pSimplified;
					}
				}
				return exp;
			}
			break;
		case MFExp_Function:
		{
			e = exp;

			size_t numExpresions = expressions.size();
			e.array.a = expressions.getPointer() + numExpresions;
			expressions.resize(numExpresions + e.array.len);

			for(size_t i=0; i<exp.array.len; ++i)
				e.array.a[i] = SimplifyExpression(exp.array.a[i], effect, pTechnique, expressions, platform);

			int c = MFString_Enumerate(e.pName, gpConstIdentifiers, sizeof(gpConstIdentifiers) / sizeof(gpConstIdentifiers[0]), true);
			if(c >= 0)
			{
				if(gIdentifierDesc[c].et == MFExp_Function)
				{
					MFDebug_Assert(exp.array.len == gIdentifierDesc[c].numArgs, "Incorrect number of arguments");

					bool bAllLiteral = true;
					for(size_t i=0; i<exp.array.len; ++i)
					{
						if(e.array.a[i].expression != MFExp_Immediate)
						{
							bAllLiteral = false;
							break;
						}
					}
					if(bAllLiteral)
					{
						if(gIdentifierDesc[c].type == MFEDT_IntVector)
						{
							MFIntExpression *pA = e.array.a;
							for(size_t i=0; i<exp.array.len; ++i)
							{
								if(pA[i].type == MFEDT_Int)
									e.array.iv[i] = pA[i].value.i;
								else
									MFDebug_Assert(false, "Incorrect data type!");
							}
							e.expression = MFExp_Immediate;
							e.type = MFEDT_IntVector;
						}
						else if(gIdentifierDesc[c].type == MFEDT_FloatVector)
						{
							MFIntExpression *pA = e.array.a;
							for(size_t i=0; i<exp.array.len; ++i)
							{
								if(pA[i].type == MFEDT_Float)
									e.array.fv[i] = pA[i].value.f;
								else if(pA[i].type == MFEDT_Int)
									e.array.fv[i] = (float)pA[i].value.i;
								else
									MFDebug_Assert(false, "Incorrect data type!");
							}
							e.expression = MFExp_Immediate;
							e.type = MFEDT_FloatVector;
						}
					}
				}
			}
			break;
		}
		case MFExp_LogicalNot:
		{
			MFIntExpression r = SimplifyExpression(*(MFIntExpression*)exp.op.pRight, effect, pTechnique, expressions, platform);
			if(r.expression == MFExp_Immediate)
			{
				e.expression = MFExp_Immediate;
				e.type = MFEDT_Bool;
				switch(r.type)
				{
					case MFEDT_Int:		e.value.b = !r.value.i;						break;
					case MFEDT_Float:	e.value.b = !r.value.f;						break;
					case MFEDT_String:	e.value.b = !r.value.s || !r.value.s[0];	break;
					case MFEDT_Bool:	e.value.b = !r.value.b;						break;
					default:			MFDebug_Assert(false, "Invalid type");		break;
				}
			}
			else
			{
				e = exp;
				e.op.pRight = &expressions.push(r);
			}
			break;
		}
		case MFExp_Pos:
		{
			MFIntExpression r = SimplifyExpression(*(MFIntExpression*)exp.op.pRight, effect, pTechnique, expressions, platform);
			MFDebug_Assert(r.type <= MFEDT_FloatVector, "Can't operate on type");
			return r;
		}
		case MFExp_Neg:
		{
			MFIntExpression r = SimplifyExpression(*(MFIntExpression*)exp.op.pRight, effect, pTechnique, expressions, platform);
			if(r.expression == MFExp_Immediate)
			{
				e.expression = MFExp_Immediate;
				e.type = MFEDT_Bool;
				switch(r.type)
				{
					case MFEDT_Int:			e.value.i = -r.value.i;					break;
					case MFEDT_Float:		e.value.f = -r.value.f;					break;
					case MFEDT_IntVector:	for(size_t i=0; i<e.array.len; ++i) e.array.iv[i] = -r.array.iv[i];	break;
					case MFEDT_FloatVector:	for(size_t i=0; i<e.array.len; ++i) e.array.fv[i] = -r.array.fv[i];	break;
					default:				MFDebug_Assert(false, "Invalid type");	break;
				}
			}
			else
			{
				e = exp;
				e.op.pRight = &expressions.push(r);
			}
			break;
		}
		case MFExp_Member:
		{
			MFIntExpression l = SimplifyExpression(*(MFIntExpression*)exp.op.pLeft, effect, pTechnique, expressions, platform);
			MFIntExpression r = SimplifyExpression(*(MFIntExpression*)exp.op.pRight, effect, pTechnique, expressions, platform);
			MFDebug_Assert(r.expression == MFExp_Identifier, "Expected identifier");
			if(l.expression == MFExp_Immediate)
			{
				int index = -1;
				if(!MFString_Compare(r.pName, "x")) index = 0;
				else if(!MFString_Compare(r.pName, "y")) index = 1;
				else if(!MFString_Compare(r.pName, "z")) index = 2;
				else if(!MFString_Compare(r.pName, "w")) index = 3;

				if(index > -1)
				{
					e.expression = MFExp_Immediate;
					switch(r.type)
					{
						case MFEDT_IntVector:	e.type = MFEDT_Int;		e.value.i = l.array.iv[index];	break;
						case MFEDT_FloatVector:	e.type = MFEDT_Float;	e.value.f = l.array.fv[index];	break;
						default:				MFDebug_Assert(false, "Is not vector type");		break;
					}
				}
				else if(!MFString_Compare(r.pName, "length"))
				{
					MFDebug_Assert(gDataTypeProperties[e.type] & DTP_HasMembers, "Does not have length property");
					e.expression = MFExp_Immediate;
					e.type = MFEDT_Int;
					e.value.i = (int)e.array.len;
				}
				else
					MFDebug_Assert(false, "Unknown property");
			}
			else
			{
				e = exp;
				e.op.pLeft = &expressions.push(l);
				e.op.pRight = &expressions.push(r);
			}
			break;
		}
		case MFExp_LogicalAnd:
		case MFExp_LogicalOr:
		{
			MFIntExpression l = SimplifyExpression(*(MFIntExpression*)exp.op.pLeft, effect, pTechnique, expressions, platform);
			MFIntExpression r = SimplifyExpression(*(MFIntExpression*)exp.op.pRight, effect, pTechnique, expressions, platform);
			int lb = -1, rb = -1;
			if(l.expression == MFExp_Immediate)
			{
				switch(l.type)
				{
					case MFEDT_Int:		lb = l.value.i != 0;					break;
					case MFEDT_Float:	lb = l.value.f != 0.f;					break;
					case MFEDT_String:	lb = l.value.s && l.value.s[0];			break;
					case MFEDT_Bool:	lb = l.value.b;							break;
					default:			MFDebug_Assert(false, "Invalid type");	break;
				}
			}
			if(r.expression == MFExp_Immediate)
			{
				switch(r.type)
				{
					case MFEDT_Int:		rb = r.value.i != 0;					break;
					case MFEDT_Float:	rb = r.value.f != 0.f;					break;
					case MFEDT_String:	rb = r.value.s && r.value.s[0];			break;
					case MFEDT_Bool:	rb = r.value.b;							break;
					default:			MFDebug_Assert(false, "Invalid type");	break;
				}
			}

			if(lb != -1 && rb != -1)
			{
				e.expression = MFExp_Immediate;
				e.type = MFEDT_Bool;
				e.value.b = exp.expression == MFExp_LogicalAnd ? (lb && rb) : (lb || rb);
			}
			else if(exp.expression == MFExp_LogicalAnd && (lb == 0 || rb == 0))
			{
				e.expression = MFExp_Immediate;
				e.type = MFEDT_Bool;
				e.value.b = false;
			}
			else if(exp.expression == MFExp_LogicalOr && (lb == 1 || rb == 1))
			{
				e.expression = MFExp_Immediate;
				e.type = MFEDT_Bool;
				e.value.b = true;
			}
			else if(exp.expression == MFExp_LogicalAnd && (lb == 1 || rb == 1))
			{
				e = lb == 1 ? r : l;
			}
			else if(exp.expression == MFExp_LogicalOr && (lb == 0 || rb == 0))
			{
				e = lb == 0 ? r : l;
			}
			else
			{
				e = exp;
				e.op.pLeft = &expressions.push(l);
				e.op.pRight = &expressions.push(r);
			}
			break;
		}
		case MFExp_NotEquiv:
		case MFExp_Equiv:
		{
			MFIntExpression l = SimplifyExpression(*(MFIntExpression*)exp.op.pLeft, effect, pTechnique, expressions, platform);
			MFIntExpression r = SimplifyExpression(*(MFIntExpression*)exp.op.pRight, effect, pTechnique, expressions, platform);
			MFDebug_Assert(l.type == r.type || (l.type < MFEDT_String && r.type < MFEDT_String && (l.type ^ r.type) != 2), "Incorrect type");
			if(l.expression == MFExp_Immediate && r.expression == MFExp_Immediate)
			{
				e.expression = MFExp_Immediate;
				e.type = MFEDT_Bool;
				switch(l.type)
				{
					case MFEDT_Int:
						e.value.b = r.type == MFEDT_Int ? (l.value.i == r.value.i) : ((float)l.value.i == r.value.f);
						break;
					case MFEDT_Float:
						e.value.b = r.type == MFEDT_Int ? (l.value.f == (float)r.value.i) : (l.value.f == r.value.f);
						break;
					case MFEDT_IntVector:
					case MFEDT_FloatVector:
						MFDebug_Assert(false, "Todo");
						break;
					case MFEDT_String:
						e.value.b = !MFString_CaseCmp(l.value.s, r.value.s);
						break;
					case MFEDT_Bool:
						e.value.b = l.value.b == r.value.b;
						break;
					default:
						MFDebug_Assert(false, "Incorrect type");
						break;
				}
			}
			else
			{
				e = exp;
				e.op.pLeft = &expressions.push(l);
				e.op.pRight = &expressions.push(r);
			}
			break;
		}
		case MFExp_LessEquiv:
		case MFExp_GreaterEquiv:
		case MFExp_Less:
		case MFExp_Greater:
		{
			MFIntExpression l = SimplifyExpression(*(MFIntExpression*)exp.op.pLeft, effect, pTechnique, expressions, platform);
			MFIntExpression r = SimplifyExpression(*(MFIntExpression*)exp.op.pRight, effect, pTechnique, expressions, platform);
			MFDebug_Assert(l.type < MFEDT_IntVector && r.type < MFEDT_IntVector, "Incorrect type");
			if(l.expression == MFExp_Immediate && r.expression == MFExp_Immediate)
			{
				e.expression = MFExp_Immediate;
				e.type = MFEDT_Bool;
				switch(l.type)
				{
					case MFEDT_Int:
						e.value.b = r.type == MFEDT_Int ? (l.value.i < r.value.i) : ((float)l.value.i < r.value.f);
						break;
					case MFEDT_Float:
						e.value.b = r.type == MFEDT_Int ? (l.value.f < (float)r.value.i) : (l.value.f < r.value.f);
						break;
					default:
						MFDebug_Assert(false, "Incorrect type");
						break;
				}
			}
			else
			{
				e = exp;
				e.op.pLeft = &expressions.push(l);
				e.op.pRight = &expressions.push(r);
			}
			break;
		}
		case MFExp_Assignment:
		{
			MFIntExpression l = SimplifyExpression(*(MFIntExpression*)exp.op.pLeft, effect, pTechnique, expressions, platform);
			MFIntExpression r = SimplifyExpression(*(MFIntExpression*)exp.op.pRight, effect, pTechnique, expressions, platform);
			e = exp;
			e.op.pLeft = &expressions.push(l);
			e.op.pRight = &expressions.push(r);
			break;
		}
		case MFExp_Add:
		case MFExp_Sub:
		case MFExp_Mul:
		case MFExp_Div:
		{
			MFIntExpression l = SimplifyExpression(*(MFIntExpression*)exp.op.pLeft, effect, pTechnique, expressions, platform);
			MFIntExpression r = SimplifyExpression(*(MFIntExpression*)exp.op.pRight, effect, pTechnique, expressions, platform);
			MFDebug_Assert(l.type < MFEDT_String && r.type < MFEDT_String && (l.type ^ r.type) != 2, "Incorrect type");
			MFDebug_Assert(false, "Todo");
			break;
		}
		default:
			MFUNREACHABLE;
			break;
	}

	return e;
}

static bool GetBoolExpression(MFIntExpression &e, int *pBool, bool *pVal)
{
	if(e.expression == MFExp_Identifier)
	{
		for(int i=0; i<MFSCB_Max; ++i)
		{
			const char *pName = MFStateBlock_GetRenderStateName(MFSB_CT_Bool, i);
			if(!MFString_Compare(e.pName, pName))
			{
				*pBool = i;
				*pVal = true;
				return true;
			}
		}
	}
	else if(e.expression == MFExp_LogicalNot)
	{
		bool isBool = GetBoolExpression(*e.op.pRight, pBool, pVal);
		if(isBool)
			*pVal = !*pVal;
		return isBool;
	}
	else if(e.expression == MFExp_Equiv || e.expression == MFExp_NotEquiv)
	{
		bool isBool = GetBoolExpression(*e.op.pLeft, pBool, pVal);
		if(isBool)
		{
			if(e.op.pRight->expression == MFExp_Immediate && e.op.pRight->type == MFEDT_Bool)
			{
				*pVal = e.op.pRight->value.b == (e.expression == MFExp_Equiv ? *pVal : !*pVal);
				return true;
			}
		}
		else
		{
			isBool = GetBoolExpression(*e.op.pRight, pBool, pVal);
			if(isBool)
			{
				if(e.op.pLeft->expression == MFExp_Immediate && e.op.pLeft->type == MFEDT_Bool)
				{
					*pVal = e.op.pLeft->value.b == (e.expression == MFExp_Equiv ? *pVal : !*pVal);
					return true;
				}
			}
		}
	}

	return false;
}

static void FlattenSelection(MFIntExpression &e, MFEffectTechnique &t)
{
	// TODO: cull the flattened terms from the expression...

	if(e.expression == MFExp_LogicalAnd)
	{
		FlattenSelection(*e.op.pLeft, t);
		FlattenSelection(*e.op.pRight, t);
	}
	else
	{
		int boolIndex;
		bool val;
		if(GetBoolExpression(e, &boolIndex, &val))
		{
			int bit = 1 << boolIndex;
			t.bools |= bit;
			t.boolValue = (val ? 1 : 0) << boolIndex | (t.boolValue & ~bit);
		}
	}
}

MF_API void MFIntEffect_CreateRuntimeData(MFIntEffect *pEffect, MFEffect **ppOutputEffect, size_t *pSize, MFPlatform platform, size_t extraBytes)
{
	MFArray<MFIntExpression> expressions;
	expressions.reserve(pEffect->expressions.size());

	for(size_t i=0; i<pEffect->variables.size(); ++i)
	{
		MFIntExpression s = SimplifyExpression(*pEffect->variables[i].pValue, *pEffect, NULL, expressions, platform);
		pEffect->variables[i].pSimplified = &expressions.push(s);
	}

	int numTechniques = 0;
	for(size_t i=0; i<pEffect->techniques.size(); ++i)
	{
		MFIntEffect::Technique &t = pEffect->techniques[i];
		t.bIncluded = true;

		for(size_t j=0; j<t.variables.size(); ++j)
		{
			MFIntExpression s = SimplifyExpression(*t.variables[j].pValue, *pEffect, &t, expressions, platform);
			t.variables[j].pSimplified = &expressions.push(s);
		}

		if(t.pSelection)
		{
			MFIntExpression s = SimplifyExpression(*pEffect->techniques[i].pSelection, *pEffect, &t, expressions, platform);
			if(s.expression == MFExp_Immediate && s.type == MFEDT_Bool && !s.value.b)
			{
				t.bIncluded = false;
				continue;
			}

			t.pSimplified = &expressions.push(s);
		}

		++numTechniques;

		for(int j=0; j<MFST_Max; ++j)
		{
			if(t.shaders[j] != NULL)
			{
				MFIntExpression s = SimplifyExpression(*t.shaders[j], *pEffect, &t, expressions, platform);
				t.simplifiedShaders[j] = &expressions.push(s);
			}
		}
	}

	size_t stringSize = MFStringCache_GetSize(pEffect->pStringCache);
	size_t size = sizeof(MFEffect) + numTechniques*sizeof(MFEffectTechnique);
	MFEffect *pFx = (MFEffect*)MFHeap_AllocAndZero(size + stringSize + extraBytes);
	pFx->pTechniques = (MFEffectTechnique*)&pFx[1];

	MFStringCache *pSC = MFStringCache_Create(stringSize);

	// build output effect
	if(pEffect->pName)
		pFx->pEffectName = MFStringCache_Add(pSC, pEffect->pName);

	numTechniques = 0;
	for(size_t i=0; i<pEffect->techniques.size(); ++i)
	{
		MFIntEffect::Technique &t = pEffect->techniques[i];
		if(!t.bIncluded)
			continue;

		MFEffectTechnique &et = pFx->pTechniques[numTechniques++];

		if(t.pName)
			et.pName = MFStringCache_Add(pSC, t.pName);

		if(t.pSimplified)
			FlattenSelection(*t.pSimplified, et);

		for(int j=0; j<MFST_Max; ++j)
		{
			MFIntExpression *pE = t.simplifiedShaders[j];
			if(!pE)
				continue;

			if(pE->expression == MFExp_Immediate && (pE->type == MFEDT_String || pE->type == MFEDT_Code))
			{
				et.shaders[j].pShaderSource = MFStringCache_Add(pSC, pE->value.code.pCode);
				et.shaders[j].pShaderLanguage = pE->value.code.pLanguage ? MFStringCache_Add(pSC, pE->value.code.pLanguage) : NULL;
				et.shaders[j].bFromFile = pE->type == MFEDT_String;
				et.shaders[j].startLine = pE->line;
			}
			else
			{
				MFDebug_Assert(false, "Expected shader source, or filename");
			}
		}
	}
	pFx->numTechniques = numTechniques;

	char *pStrings = (char*)&pFx->pTechniques[numTechniques];
	const char *pStringCache = MFStringCache_GetCache(pSC);
	stringSize = MFStringCache_GetSize(pSC);
	MFCopyMemory(pStrings, pStringCache, stringSize);
	size += stringSize;

	// fix-down all the pointers...
	uintp base = (uintp)pFx;
	uintp stringBase = (uintp)pStringCache - ((uintp)pStrings - (uintp)pFx);

	if(pFx->pEffectName)
		(char*&)pFx->pEffectName -= stringBase;
	for(int i=0; i<pFx->numTechniques; ++i)
	{
		MFEffectTechnique &et = pFx->pTechniques[i];
		if(et.pName)
			(char*&)et.pName -= stringBase;
		for(int j=0; j<MFST_Max; ++j)
		{
			if(et.shaders[j].pShaderSource)
				(char*&)et.shaders[j].pShaderSource -= stringBase;
			if(et.shaders[j].pShaderLanguage)
				(char*&)et.shaders[j].pShaderLanguage -= stringBase;
		}
		if(et.pMacros)
			(char*&)et.pMacros -= base;
	}
	(char*&)pFx->pTechniques -= base;

	MFStringCache_Destroy(pSC);

	// return it to the caller
	*ppOutputEffect = pFx;
	*pSize = size;
}
