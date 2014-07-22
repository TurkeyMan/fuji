module fuji.fuji;

public import fuji.c.Fuji;

public import fuji.string;
public import fuji.dbg;

import std.traits;

nothrow:
@nogc:

void MFEndian_Flip(T)(T* pData)
{
	static if(is(T == struct))
	{
		foreach(ref m; (*pData).tupleof)
		{
			alias M = typeof(m);
			static if(M.sizeof > 1 && (is(M == struct) || std.traits.isNumeric!M || std.traits.isSomeChar!M))
				MFEndian_Flip(&m);
		}
	}
	else
	{
		T copy = *pData;

		ubyte* pBytes = cast(ubyte*)pData;
		const(ubyte)* pCopy = cast(const(ubyte)*)&copy;
		foreach(a; 0 .. T.sizeof)
			pBytes[a] = pCopy[T.sizeof-1-a];
	}
}

template MFEndian_Flip(alias data)
{
	// TODO: this is tricky!
	static assert("TODO!");
}

version(LittleEndian)
{
	void MFEndian_HostToBig(T)(T* x) { MFEndian_Flip(x); }
	void MFEndian_HostToLittle(T)(T* x) {}
	void MFEndian_LittleToHost(T)(T* x) {}
	void MFEndian_BigToHost(T)(T* x) { MFEndian_Flip(x); }

	alias MFEndian_HostToBig(alias x) = MFEndian_Flip!x;
	alias MFEndian_HostToLittle(alias x) = x;
	alias MFEndian_LittleToHost(alias x) = x;
	alias MFEndian_BigToHost(alias x) = MFEndian_Flip!x;
}
else version(BigEndian)
{
	void MFEndian_HostToBig(T)(T* x) {}
	void MFEndian_HostToLittle(T)(T* x) { MFEndian_Flip(x); }
	void MFEndian_LittleToHost(T)(T* x) { MFEndian_Flip(x); }
	void MFEndian_BigToHost(T)(T* x) {}

	alias MFEndian_HostToBig(alias x) = x;
	alias MFEndian_HostToLittle(alias x) = MFEndian_Flip!x;
	alias MFEndian_LittleToHost(alias x) = MFEndian_Flip!x;
	alias MFEndian_BigToHost(alias x) = x;
}
else
	static assert("Unknown endian!");

void MFFixUp(ref void* pPointer, void* pBase, int fix)
{
	if(pPointer)
	{
		ptrdiff_t offset = cast(ptrdiff_t)pBase;

		if(!fix)
			offset = -offset;

		pPointer = cast(void*)(cast(ubyte*)pPointer + offset);
	}
}

T MFMin(T)(T a, T b)
{
	return a < b ? a : b;
}

T MFMax(T)(T a, T b)
{
	return a > b ? a : b;
}

T MFClamp(T)(T x, T y, T z)
{
	return MFMax(x, MFMin(y, z));
}
