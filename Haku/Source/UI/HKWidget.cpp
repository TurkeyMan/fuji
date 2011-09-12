#include "Haku.h"
#include "UI/HKWidget.h"
#include "UI/HKUI.h"

#include "MFTypes.h"
#include "MFCollision.h"

HKWidget::HKWidget()
{
	pTypeName = "HKWidget";

	pos = MFVector::zero;
	size.Set(60, 40, 0);
	colour = MFVector::white;
	scale = MFVector::one;
	rot = MFVector::zero;

	pRenderer = NULL;
	pParent = NULL;

	zDepth = 0;

	bVisible = true;
	bEnabled = true;
	bParentEnabled = true;

	bMatrixDirty = bInvMatrixDirty = true;
}

HKWidget::~HKWidget()
{
	if(pRenderer)
	{
		delete pRenderer;
		pRenderer = NULL;
	}
}

HKWidget *HKWidget::Create()
{
	return new HKWidget();
}

void HKWidget::Update()
{
	// update the children
	int numChildren = GetNumChildren();
	for(int a=0; a<numChildren; ++a)
	{
		HKWidget *pChild = GetChild(a);
		pChild->Update();
	}
}

const MFMatrix &HKWidget::GetTransform()
{
	if(bMatrixDirty)
	{
		if(rot == MFVector::zero)
		{
			matrix.SetScale(scale);
		}
		else
		{
			// build the axiis from the rotation vector
			matrix.SetRotationYPR(rot.y, rot.x, rot.z);

			// scale the axiis
			matrix.SetXAxis3(matrix.GetXAxis() * scale.x);
			matrix.SetYAxis3(matrix.GetYAxis() * scale.y);
			matrix.SetZAxis3(matrix.GetZAxis() * scale.z);
		}

		// and set the position
		matrix.SetTrans3(pos);

		// and multiply in the parent
		if(pParent)
			matrix.Multiply(pParent->GetTransform(), matrix);

		bMatrixDirty = false;
	}

	return matrix;
}

const MFMatrix &HKWidget::GetInvTransform()
{
	if(bInvMatrixDirty)
	{
		invMatrix.Inverse(GetTransform());
		bInvMatrixDirty = false;
	}
	return invMatrix;
}

void HKWidget::SetRenderer(HKWidgetRenderer *pRenderer)
{
	this->pRenderer = pRenderer;
}

HKUserInterface &HKWidget::GetUI()
{
	return HKUserInterface::Get();
}

int HKWidget::GetNumChildren() const
{
	return 0;
}

HKWidget *HKWidget::GetChild(int index) const
{
	return NULL;
}

bool HKWidget::SetEnabled(bool bEnable)
{
	bool bOld = bEnabled;
	if(bEnabled != bEnable)
	{
		bEnabled = bEnable;

		if(!OnEnabledChanged.IsEmpty())
		{
			HKWidgetEnabledEvent ev(this, bEnable);
			OnEnabledChanged(*this, ev);
		}
	}
	return bOld;
}

bool HKWidget::SetVisible(bool bVisible)
{
	bool bOld = this->bVisible;
	if(this->bVisible != bVisible)
	{
		this->bVisible = bVisible;

		if(!OnEnabledChanged.IsEmpty())
		{
			HKWidgetVisibilityEvent ev(this, bVisible);
			OnVisibleChanged(*this, ev);
		}
	}
	return bOld;
}

void HKWidget::SetPosition(const MFVector &position)
{
	if(position != pos)
	{
		MFVector oldPos = pos;
		pos = position;

		DirtyMatrices();

		if(!OnMove.IsEmpty())
		{
			HKWidgetMoveEvent ev(this);
			ev.oldPos = oldPos;
			ev.newPos = position;
			OnMove(*this, ev);
		}
	}
}

void HKWidget::SetSize(const MFVector &size)
{
	if(this->size != size)
	{
		MFVector oldSize = this->size;
		this->size = size;

		if(!OnMove.IsEmpty())
		{
			HKWidgetResizeEvent ev(this);
			ev.oldSize = oldSize;
			ev.newSize = size;
			OnResize(*this, ev);
		}
	}
}

void HKWidget::SetColour(const MFVector &colour)
{
	this->colour = colour;
}

void HKWidget::SetScale(const MFVector &scale)
{
	if(this->scale != scale)
	{
		this->scale = scale;

		DirtyMatrices();
	}
}

void HKWidget::SetRotation(const MFVector &rotation)
{
	if(this->rot != rotation)
	{
		this->rot = rotation;

		DirtyMatrices();
	}
}

void HKWidget::Draw()
{
	if(!bVisible)
		return;

	pRenderer->Render(*this, GetTransform());

	int numChildren = GetNumChildren();
	for(int a=0; a<numChildren; ++a)
	{
		HKWidget *pChild = GetChild(a);
		pChild->Draw();
	}
}

void HKWidget::DirtyMatrices()
{
	bMatrixDirty = bInvMatrixDirty = true;

	int numChildren = GetNumChildren();
	for(int a=0; a<numChildren; ++a)
		GetChild(a)->DirtyMatrices();
}

HKWidget *HKWidget::IntersectWidget(const MFVector &pos, const MFVector &dir, MFVector *pLocalPos)
{
	if(!bVisible)
		return NULL;

	if(size.z == 0.f)
	{
		// the widget is 2d, much easier

		// build a plane from the matrix
		MFVector plane;
		plane = -GetTransform().GetZAxis();

		// normalise if the plane is scaled along z
		if(scale.z != 1.f)
			plane *= 1.f/scale.z;

		// calculate w
		plane.w = -this->pos.Dot3(plane);

		MFRayIntersectionResult res;
		if(MFCollision_RayPlaneTest(pos, dir, plane, &res))
		{
			MFVector intersection = GetInvTransform().TransformVectorH(pos + dir*res.time);

			if(pLocalPos)
				*pLocalPos = intersection;

			MFRect rect =
			{
				0.f, 0.f,
				size.x, size.y
			};
			if(MFTypes_PointInRect(intersection.x, intersection.y, &rect))
			{
				HKWidget *pIntersect = this;

				int numChildren = GetNumChildren();
				for(int a=0; a<numChildren; ++a)
				{
					HKWidget *pChild = GetChild(a)->IntersectWidget(pos, dir, pLocalPos);
					if(pChild)
					{
						pIntersect = pChild;
						break;
					}
				}

				return pIntersect;
			}
		}
	}
	else
	{
		// intersect the 3d widgets cubic boundary
		//...
	}

	return NULL;
}

bool HKWidget::InputEvent(HKInputManager &manager, HKInputManager::EventInfo &ev)
{
	// try and handle the input event in some standard ways...
	switch(ev.ev)
	{
		case HKInputManager::IE_Down:
		{
			HKWidgetInputEvent ie(this, ev.pSource);
			OnDown(*this, ie);
			return true;
		}
		case HKInputManager::IE_Up:
		{
			HKWidgetInputEvent ie(this, ev.pSource);
			OnUp(*this, ie);
			return true;
		}
		case HKInputManager::IE_Tap:
		{
			HKWidgetInputEvent ie(this, ev.pSource);
			OnTap(*this, ie);
			return true;
		}
		case HKInputManager::IE_Hover:
		{
			HKWidgetInputActionEvent ie(this, ev.pSource);
			ie.pos = MakeVector(ev.hover.x, ev.hover.y);
			ie.delta = MakeVector(ev.hover.deltaX, ev.hover.deltaY);
			OnHover(*this, ie);
			return true;
		}
		case HKInputManager::IE_Drag:
		{
			HKWidgetInputActionEvent ie(this, ev.pSource);
			ie.pos = MakeVector(ev.hover.x, ev.hover.y);
			ie.delta = MakeVector(ev.hover.deltaX, ev.hover.deltaY);
			OnDrag(*this, ie);
			return true;
		}
		case HKInputManager::IE_Pinch:
		case HKInputManager::IE_Spin:
		case HKInputManager::IE_ButtonTriggered:
		case HKInputManager::IE_ButtonDown:
		case HKInputManager::IE_ButtonUp:
		case HKInputManager::IE_Wheel:
		default:
			break;
	}

	if(pParent)
		return pParent->InputEvent(manager, ev);
	return false;
}
