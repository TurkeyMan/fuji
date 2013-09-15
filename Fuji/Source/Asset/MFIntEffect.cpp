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

struct MFIntEffect
{
	struct Technique
	{
		struct Variable
		{
			DString name;
			DString value;
		};

		DString name;

		DString shader[MFST_Max];
		DString shaderCode[MFST_Max];


		// selection criteria
		//...

	};

	const char *pText;

	Technique *pTechniques;
	int numTechniques;
};

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
		ET_EndStatement,
		ET_ScopeEnd,
		ET_GroupEnd,
		ET_ArrayEnd,
	ET_EndExpressionEnd,

	ET_UnaryExpression,
		ET_LogicalNot,
		ET_Pos,
		ET_Neg,
	ET_UnaryExpressionEnd,

	ET_BinaryExpression,
		ET_Member,
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
	ET_BinaryExpressionEnd,

	ET_Terms,
		ET_Identifier,
		ET_Number,
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
	ExpressionType type;
	Expression *pLeft, *pRight;
	Token *pToken;
};

struct Statement
{
	Expression *pExpression;
	MFArray<Statement> children;
};

static const char *gpKeywords[] =
{
	"effect",
	"technique",
	"code"
};


// **** fuctions ****

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

Expression *GetExpression(int precedence, DSlice<Token> &tokens, MFArray<Expression> &expressions);

Expression *GetScope(DSlice<Token> &tokens, MFArray<Expression> &expressions)
{
	// a scope is a sequence of expressions...
	Expect(tokens, ET_Scope);

	Expression *scope = NULL;
	Expression *prev = NULL;
	while(tokens.length && tokens[0].type != ET_ScopeEnd)
	{
		Expression *s = &expressions.push();
		s->pToken = NULL;
		s->type = ET_Scope;
		s->pLeft = GetExpression(0, tokens, expressions);
		s->pRight = NULL;

		if(!scope)
			scope = s;
		if(prev)
			prev->pRight = s;
		prev = s;
	}

	Expect(tokens, ET_ScopeEnd);
	return scope;
}

Expression *GetTerm(DSlice<Token> &tokens, MFArray<Expression> &expressions)
{
	if(tokens.length == 0)
		return NULL;

	Token &next = tokens[0];
	tokens = tokens.popFront();

	if(next.type == ET_Identifier && next.token == "effect")
	{
		Token *pIdentifier = Expect(tokens, ET_Identifier);

		Expression *e = &expressions.push();
		e->pToken = pIdentifier;
		e->type = ET_Effect;
		e->pLeft = NULL;
		e->pRight = GetScope(tokens, expressions);
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
			c = GetExpression(0, tokens, expressions);
			Expect(tokens, ET_GroupEnd);
		}

		Expression *e = &expressions.push();
		e->pToken = pIdentifier;
		e->type = ET_Technique;
		e->pLeft = c;
		e->pRight = GetScope(tokens, expressions);
		return e;
	}
	else if(next.type == ET_Add || next.type == ET_Sub || next.type == ET_LogicalNot)
	{
		Expression *e = &expressions.push();
		e->pToken = &next;
		e->type = next.type;
		e->pLeft = NULL;
		e->pRight = GetExpression(next.precedence, tokens, expressions);
		return e;
	}
	else if(next.type == ET_Group)
	{
		Expression *e = GetExpression(0, tokens, expressions);
		Expect(tokens, ET_GroupEnd);
		return e;
	}
	else if(next.type > ET_Terms && next.type < ET_TermsEnd)
	{
		Expression *e = &expressions.push();
		e->pToken = &next;
		e->type = next.type;
		e->pLeft = e->pRight = NULL;

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
						Expression *a = &expressions.push();
						a->pToken = NULL;
						a->type = ET_ListItem;
						a->pLeft = GetExpression(0, tokens, expressions);
						a->pRight = NULL;

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

Expression *GetExpression(int precedence, DSlice<Token> &tokens, MFArray<Expression> &expressions)
{
	Expression *e = GetTerm(tokens, expressions);

	if(tokens.length == 0)
		return e;

	Token *next = &tokens[0];
	while((next->type > ET_BinaryExpression && next->type < ET_BinaryExpressionEnd) && next->precedence >= precedence)
	{
		tokens = tokens.popFront();
/*
		const q := case associativity(op)
					of Right: prec( op )
						Left:  1+prec( op )
*/
		bool bLeftAssoc = true;
		Expression *e2 = GetExpression(next->precedence + (bLeftAssoc ? 1 : 0), tokens, expressions);

		Expression *op = &expressions.push();
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

Expression *ParseExpression(DSlice<Token> &tokens, MFArray<Expression> &expressions)
{
	Expression *e = GetExpression(0, tokens, expressions);
//	Expect(tokens, ET_EndStatement);
	return e;
}

/*
Expression *ParseExpression(Expression *pLeft, DSlice<Token> &tokens, MFArray<Expression> &expressions)
{
	if(tokens.length == 0)
		return pLeft;
	if(tokens[0].type > ET_EndExpression && tokens[0].type < ET_EndExpressionEnd)
	{
//		tokens = tokens.popFront();
		return pLeft;
	}

	Expression *e = &expressions.push();
	e->pLeft = e->pRight = NULL;
	e->pToken = &tokens[0];
	e->type = e->pToken->type;
	tokens = tokens.popFront();

	if(e->type > ET_Terms && e->type < ET_TermsEnd)
	{
		Expression *next = ParseExpression(NULL, tokens, expressions);
		if(next->type < ET_BinaryExpression || next->type > ET_BinaryExpressionEnd)
			e->pRight = next;
		e = next;
	}
	else if(e->type > ET_BinaryExpression && e->type < ET_BinaryExpressionEnd)
	{
		Expression *next = ParseExpression(NULL, tokens, expressions);
		if(next->type > ET_BinaryExpression && next->type < ET_BinaryExpressionEnd)
		{
			next->
		}
		else
			e = ParseExpression(e, tokens, expressions);
	}
	else if(e->type > ET_RangeExpression && e->type < ET_RangeExpressionEnd)
	{
		e->pLeft = ParseExpression(NULL, tokens, expressions);
	}

	return e;
}
*/
Statement ParseStatement(DSlice<Token> &tokens, MFArray<Expression> &expressions)
{
	Statement s;
	s.pExpression = ParseExpression(tokens, expressions);
	return s;
}

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
//			tokens.push(Token(buffer.slice(0, 1), ET_EndStatement, line, 0));
			++line;

			buffer = buffer.popFront();
			pLineStart = buffer.ptr;
		}
		else if(MFIsNumeric(c))
		{
			// it's	a number
			int col = (int)(buffer.ptr - pLineStart);

			size_t len = 1;

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
					continue;
				}
				else if(c == '.' || c == '_' || MFIsAlpha(c))
					return Error("error: Invalid number.", pFilename, line, col, pBuffer);
				break;
			}

			tokens.push(Token(buffer.slice(0, len), ET_Number, line, col));
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

			tokens.push(Token(buffer.slice(0, len), ET_String, line, col));
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

				tokens.push(Token(buffer.slice(0, len), ET_SourceCode, line, col));
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
	MFArray<Expression> expressions;
	MFArray<Statement> statements;

	DSlice<Token> tok(tokens.getPointer(), tokens.size());
	while(tok.length)
		statements.push(ParseStatement(tok, expressions));

	// build the MFEffect structure


	return NULL;

/*
	// not possible for a font to be bigger than 2mb..
	char *pFontFile = (char*)MFHeap_AllocAndZero(2 * 1024 * 1024);

	MFFont *pHeader = (MFFont*)pFontFile;
	pHeader->ppPages = (MFMaterial**)&pHeader[1];

	MFStringCache *pStr = MFStringCache_Create(1024*1024);

	MFFontChar *pC = (MFFontChar*)MFHeap_Alloc(sizeof(MFFontChar) * 65535);

	char *pLine, *pText = pBuffer;
	while((pLine = GetNextLine(pText)) != NULL)
	{
		char *pT = strtok(pLine, " \t");

		if(!MFString_Compare(pT, "info"))
		{
			while(pT)
			{
				if(!MFString_CompareN(pT, "face", 4))
				{
					pHeader->pName = MFStringCache_Add(pStr, strtok(NULL, "\""));
				}
				else if(!MFString_CompareN(pT, "size", 4))
				{
					pHeader->size = atoi(&pT[5]);
				}
				else if(!MFString_CompareN(pT, "bold", 4))
				{
					pHeader->flags |= atoi(&pT[5]) ? MFFF_Bold : 0;
				}
				else if(!MFString_CompareN(pT, "italic", 6))
				{
					pHeader->flags |= atoi(&pT[7]) ? MFFF_Italic : 0;
				}
				else if(!MFString_CompareN(pT, "unicode", 7))
				{
					pHeader->flags |= atoi(&pT[8]) ? MFFF_Unicode : 0;
				}
				else if(!MFString_CompareN(pT, "smooth", 6))
				{
					pHeader->flags |= atoi(&pT[7]) ? MFFF_Smooth : 0;
				}

				pT = strtok(NULL, " \"\t");
			}
		}
		else if(!MFString_Compare(pT, "common"))
		{
			while(pT)
			{
				if(!MFString_CompareN(pT, "lineHeight", 10))
				{
					pHeader->height = atoi(&pT[11]);
				}
				else if(!MFString_CompareN(pT, "base", 4))
				{
					pHeader->base = atoi(&pT[5]);
				}
				else if(!MFString_CompareN(pT, "scaleW", 6))
				{
					pHeader->xScale = 1.0f / (float)atoi(&pT[7]);
				}
				else if(!MFString_CompareN(pT, "scaleH", 6))
				{
					pHeader->yScale = 1.0f / (float)atoi(&pT[7]);
				}
				else if(!MFString_CompareN(pT, "pages", 5))
				{
					pHeader->numPages = atoi(&pT[6]);
					pHeader->pCharacterMapping = (uint16*)&pHeader->ppPages[pHeader->numPages];
				}
				else if(!MFString_CompareN(pT, "packed", 6))
				{
//					pHeader-> = atoi(&pT[5]);
				}

				pT = strtok(NULL, " \"\t");
			}
		}
		else if(!MFString_Compare(pT, "page"))
		{
			int id = -1;

			while(pT)
			{

				if(!MFString_CompareN(pT, "id", 2))
				{
					id = atoi(&pT[3]);
				}
				else if(!MFString_CompareN(pT, "file", 4))
				{
					pT = strtok(NULL, "\"");
					pT[MFString_Length(pT)-4] = 0;
					pHeader->ppPages[id] = (MFMaterial*)MFStringCache_Add(pStr, pT);
				}

				pT = strtok(NULL, " \"\t");
			}
		}
		else if(!MFString_Compare(pT, "char"))
		{
			while(pT)
			{
				if(!MFString_CompareN(pT, "id", 2))
				{
					int id = atoi(&pT[3]);
					pHeader->pCharacterMapping[id] = (uint16)pHeader->numChars;
					pHeader->maxMapping = MFMax(pHeader->maxMapping, id);
					pC[pHeader->numChars].id = (uint16)id;
				}
				else if(!MFString_CompareN(pT, "x=", 2))
				{
					pC[pHeader->numChars].x = (uint16)atoi(&pT[2]);
				}
				else if(!MFString_CompareN(pT, "y=", 2))
				{
					pC[pHeader->numChars].y = (uint16)atoi(&pT[2]);
				}
				else if(!MFString_CompareN(pT, "width", 5))
				{
					pC[pHeader->numChars].width = (uint16)atoi(&pT[6]);
				}
				else if(!MFString_CompareN(pT, "height", 6))
				{
					pC[pHeader->numChars].height = (uint16)atoi(&pT[7]);
				}
				else if(!MFString_CompareN(pT, "xoffset", 7))
				{
					pC[pHeader->numChars].xoffset = (int8)atoi(&pT[8]);
				}
				else if(!MFString_CompareN(pT, "yoffset", 7))
				{
					pC[pHeader->numChars].yoffset = (int8)atoi(&pT[8]);
				}
				else if(!MFString_CompareN(pT, "xadvance", 8))
				{
					pC[pHeader->numChars].xadvance = (uint16)atoi(&pT[9]);
				}
				else if(!MFString_CompareN(pT, "page", 4))
				{
					pC[pHeader->numChars].page = (uint8)atoi(&pT[5]);
				}
				else if(!MFString_CompareN(pT, "chnl", 4))
				{
					pC[pHeader->numChars].channel = (uint8)atoi(&pT[5]);
				}

				pT = strtok(NULL, " \"\t");
			}

			++pHeader->numChars;
		}
	}

	// append characters to file
	uint16 *pMapEnd = &pHeader->pCharacterMapping[pHeader->maxMapping+1];
	uintp offset = MFALIGN16(pMapEnd);
	pHeader->pChars = (MFFontChar*&)offset;
	MFCopyMemory(pHeader->pChars, pC, sizeof(MFFontChar) * pHeader->numChars);

	// append string cache to file...
	char *pStrings = (char*)&pHeader->pChars[pHeader->numChars];
	const char *pCache = MFStringCache_GetCache(pStr);
	size_t stringLen = MFStringCache_GetSize(pStr);
	MFCopyMemory(pStrings, pCache, stringLen);

	// byte reverse
	// TODO...

	// fix up pointers
	uintp base = (uintp)pFontFile;
	uintp stringBase = (uintp)pCache - ((uintp)pStrings - (uintp)pFontFile);

	for(int a=0; a<pHeader->numPages; a++)
		(char*&)pHeader->ppPages[a] -= stringBase;

	(char*&)pHeader->ppPages -= base;
	(char*&)pHeader->pChars -= base;
	(char*&)pHeader->pCharacterMapping -= base;
	(char*&)pHeader->pName -= stringBase;

	// write to disk
	int fileSize = (int)(((uintp)pStrings + stringLen) - (uintp)pFontFile);
	MFFont *pFont = (MFFont*)MFHeap_Alloc(fileSize);
	MFCopyMemory(pFont, pFontFile, fileSize);

	// clean up
	MFStringCache_Destroy(pStr);
	MFHeap_Free(pC);
	MFHeap_Free(pFontFile);
	MFHeap_Free(pBuffer);

	*ppOutput = pFont;
	*pSize = fileSize;
*/
}

MF_API void MFIntEffect_Destroy(MFIntEffect *pEffect)
{
}

MF_API void MFIntEffect_CreateRuntimeData(MFIntEffect *pEffect, MFEffect **ppOutputEffect, size_t *pSize, MFPlatform platform, size_t extraBytes)
{

}
