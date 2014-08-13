using System;
using System.Runtime.InteropServices;

namespace Fuji
{
	public struct MFVector
	{
		public float x, y, z, w;

		public MFVector(float f)
		{
			x = y = z = w = f;
		}

		public MFVector(float x, float y, float z = 0, float w = 0)
		{
			this.x = x;
			this.y = y;
			this.z = z;
			this.w = w;
		}

		public MFVector(MFVector xyz, float w)
		{
			x = xyz.x;
			y = xyz.y;
			z = xyz.z;
			this.w = w;
		}

		public static readonly MFVector zero =		new MFVector { x=0, y=0, z=0, w=0 };
		public static readonly MFVector one =		new MFVector { x=1, y=1, z=1, w=1 };
		public static readonly MFVector identity =	new MFVector { x=0, y=0, z=0, w=1 };
		public static readonly MFVector up = 		new MFVector { x=0, y=1, z=0, w=0 };
		public static readonly MFVector right =		new MFVector { x=1, y=0, z=0, w=0 };
		public static readonly MFVector forward =	new MFVector { x=0, y=0, z=1, w=0 };
	}
}
