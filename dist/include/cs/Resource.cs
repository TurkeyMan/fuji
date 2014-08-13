using System;
using System.Runtime.InteropServices;

namespace Fuji
{
	using MFResource = UIntPtr;
	using MFResourceIterator = UIntPtr;

	public enum MFResourceType
	{
		All = -2,
		Unknown = -1,
		Max = 32
	}

	public struct Resource
	{
		public MFResource handle;

		public static Resource Find(uint hash)
		{
			return new Resource { handle = MFResource_Find(hash) };
		}

		public static int NumResources(int type)
		{
			return MFResource_GetNumResources(type);
		}


		public int RefCount
		{
			get { return MFResource_GetRefCount(handle); }
		}

		public int Type
		{
			get { return MFResource_GetType(handle); }
		}
	
		public string TypeName
		{
			get { return MFResource_GetTypeName(MFResource_GetType(handle)); }
		}
	
		public uint Hash
		{
			get { return MFResource_GetHash(handle); }
		}
	
		public string Name
		{
			get { return MFResource_GetName(handle); }
		}
	
		public int AddRef()
		{
			return MFResource_AddRef(handle);
		}

		public int Release()
		{
			if(handle == MFResource.Zero)
				return 0;

			int rc = MFResource_Release(handle);
			handle = MFResource.Zero;
			return rc;
		}

		// TODO: AsFont(), AsModel(),  ... assert the type is correcet and cast

//		[DllImport (Engine.DllVersion)]
//		private static extern int MFResource_Register([MarshalAs(UnmanagedType.LPStr)] string name, DestroyResourceFunction *pDestroyFunc);
		[DllImport (Engine.DllVersion)]
		[return: MarshalAs(UnmanagedType.LPStr)]
		private static extern string MFResource_GetTypeName(int type);
//		[DllImport (Engine.DllVersion)]
//		private static extern void MFResource_AddResource(MFResource resource, int type, uint hash, [MarshalAs(UnmanagedType.LPStr)] string name);
		[DllImport (Engine.DllVersion)]
		private static extern MFResource MFResource_Find(uint hash);
		[DllImport (Engine.DllVersion)]
		private static extern int MFResource_GetType(MFResource resource);
		[DllImport (Engine.DllVersion)]
		private static extern uint MFResource_GetHash(MFResource resource);
		[DllImport (Engine.DllVersion)]
		private static extern int MFResource_GetRefCount(MFResource resource);
		[DllImport (Engine.DllVersion)]
		[return: MarshalAs(UnmanagedType.LPStr)]
		private static extern string MFResource_GetName(MFResource resource);
		[DllImport (Engine.DllVersion)]
		private static extern int MFResource_AddRef(MFResource resource);
		[DllImport (Engine.DllVersion)]
		private static extern int MFResource_Release(MFResource resource);
		[DllImport (Engine.DllVersion)]
		private static extern int MFResource_GetNumResources(int type = (int)MFResourceType.All);
//		[DllImport (Engine.DllVersion)]
//		private static extern MFResourceIterator MFResource_EnumerateFirst(int type = (int)MFResourceType.All);
//		[DllImport (Engine.DllVersion)]
//		private static extern MFResourceIterator MFResource_EnumerateNext(MFResourceIterator iterator, int type = (int)MFResourceType.All);
//		[DllImport (Engine.DllVersion)]
//		private static extern MFResource MFResource_Get(MFResourceIterator iterator);
	}
}
