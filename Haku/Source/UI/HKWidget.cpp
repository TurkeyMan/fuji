#include "Haku.h"
#include "UI/HKWidget.h"

#include "MFTypes.h"
#include "MFCollision.h"

HKWidget::HKWidget()
{
	pTypeName = "HKWidget";

	pos = MakeVector(0, 0, 0);
	size = MakeVector(30, 20, 0);
	colour = MFVector::white;
	scale = MFVector::one;
	rot = MFVector::zero;

	renderCallback.clear();
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
	// do something?
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
	}

	return matrix;
}

const MFMatrix &HKWidget::GetInvTransform()
{
	if(bInvMatrixDirty)
		invMatrix.Inverse(GetTransform());
	return invMatrix;
}

void HKWidget::SetRenderer(HKWidgetRenderer *pRenderer)
{
	pRenderer = pRenderer;
	renderCallback = pRenderer->GetRenderDelegate();
}

bool HKWidget::SetEnabled(bool bEnable)
{
	bool bOld = bEnabled;
	if(bEnabled != bEnable)
	{
		bEnabled = bEnable;

		if(!OnEnabledChanged.IsEmpty())
		{
			HKWidgetEnabledEvent *pEvent = (HKWidgetEnabledEvent*)HKWidgetEventInfo::Alloc(this);
			pEvent->bEnabled = bEnable;
			OnEnabledChanged(*this, pEvent);
			HKWidgetEventInfo::Free(pEvent);
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
			HKWidgetVisibilityEvent *pEvent = (HKWidgetVisibilityEvent*)HKWidgetEventInfo::Alloc(this);
			pEvent->bVisible = bVisible;
			OnVisibleChanged(*this, pEvent);
			HKWidgetEventInfo::Free(pEvent);
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

		if(!OnMove.IsEmpty())
		{
			HKWidgetMoveEvent *pEvent = (HKWidgetMoveEvent*)HKWidgetEventInfo::Alloc(this);
			pEvent->oldPos = oldPos;
			pEvent->newPos = position;
			OnMove(*this, pEvent);
			HKWidgetEventInfo::Free(pEvent);
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
			HKWidgetResizeEvent *pEvent = (HKWidgetResizeEvent*)HKWidgetEventInfo::Alloc(this);
			pEvent->oldSize = oldSize;
			pEvent->newSize = size;
			OnResize(*this, pEvent);
			HKWidgetEventInfo::Free(pEvent);
		}
	}
}

void HKWidget::SetColour(const MFVector &colour)
{
	this->colour = colour;
}

void HKWidget::SetScale(const MFVector &scale)
{
	this->scale = scale;
}

void HKWidget::SetRotation(const MFVector &rotation)
{
	this->rot = rotation;
}

bool HKWidget::IntersectWidget(const MFVector &pos, const MFVector &dir)
{
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
			MFRect rect =
			{
				0.f, 0.f,
				size.x, size.y
			};
			if(MFTypes_PointInRect(intersection.x, intersection.y, &rect))
				return true;
		}
	}
	else
	{
		// intersect the 3d widgets cubic boundary
		//...
	}

	return false;
}
