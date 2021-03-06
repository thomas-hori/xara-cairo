/*
 * xara-cairo, a vector drawing program
 *
 * Based on Xara LX, Copyright (C) 1993-2006 Xara Group Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, version 2.
 *
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

// NodeBlender class. 

#ifndef INC_NODEBLDR
#define INC_NODEBLDR

#include "blendatt.h"

class RenderRegion;
class Spread;
class BecomeA;
class HandleBecomeA;
class CCAttrMap;
class Progress;
class BaseCamelotFilter;
class NodeBlendPath;
class NodeBlend;
class Trans2DMatrix;
class BevelNodeParam;
class NodeCompound;
class BitmapFillAttribute;
class AttrBitmapFill;
class NodeGroup;
class SumAllPathsElem;

/***********************************************************************************************

>	class BlendPath: public ListItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Purpose:	This contains a NodePath, plus other data, generated by an object that is blended

***********************************************************************************************/

class BlendPath: public ListItem
{
	CC_DECLARE_MEMDUMP(BlendPath);
public:
	BlendPath();
	~BlendPath();

	// DMc change
	// previously this operated purely on node paths (see the commented out areas)
	// however, to cope with compound nodes such as contours, bevels & shadows this
	// is now impossible so it has to work on NodeRenderableInks

	/*
	BOOL				Initialise(	NodePath* pThisNodePath,
									INT32 Index,
									NodeRenderableInk* pThisCreatedByNode,
									UINT32 SubpathID,
									UINT32 SubPathNum,
									CCAttrMap* pAttrMap);
									*/

	BOOL				Initialise(	NodeRenderableInk* pThisBlendNode,
									INT32 Index,
									NodeRenderableInk* pThisCreatedByNode,
									UINT32 SubpathID,
									UINT32 SubPathNum,
									CCAttrMap* pAttrMap,
									BOOL CheckDirection = TRUE);

	// DMc
	// alternative initialisation function, replacing the blend node & path
	BOOL				Initialise( BlendPath *pBlendPath, NodeRenderableInk * pNodeToBlend);	

	// NodeRenderableInk*	GetNodePath() 		{ return m_pNodePath; }
	NodeRenderableInk*	GetBlendNode() 		{ return m_pBlendNode; }
	Path*				GetPath() 			{ return m_pPath; }
	NodeRenderableInk*	GetCreatedByNode() 	{ return m_pCreatedByNode; }
	INT32				GetOrigMapping() 	{ return m_OrigMapping; }
	PathTypeEnum 		GetPathType() 		{ return m_PathType; }
	UINT32				GetSubPathID() 		{ return m_SubPathID; }
	UINT32				GetSubPathNum() 	{ return m_SubPathNum; }
	INT32				GetNumElements() 	{ return m_NumElements; }
	BOOL				UniqueAppliedAttrs(){ return m_UniqueAttrs; }
	MILLIPOINT			GetAppliedLineWidth();

	BOOL				IsLine()  { return m_PathType == PATHTYPE_LINE;  }
	BOOL				IsShape() { return m_PathType == PATHTYPE_SHAPE; }

	void 				Transform( TransformBase& Trans );
	void 				RenderBlendBlobs(RenderRegion* pRender,Trans2DMatrix* pMatrix);
	BOOL				IsFilled();
	BOOL				IsStroked();
	DocCoord			GetPathCoord(INT32 Index);
	BOOL 				IsPointOverBlob(DocCoord* pPointerPos,INT32* pIndex,Trans2DMatrix* pMatrix);
	INT32				MapPath(INT32 Index);
	DocCoord			MapPath(DocCoord Index);
	INT32				FindBottomLeft();

	CCAttrMap*			FindAppliedAttributes();
	void				FreeAppliedAttributes();
	AttrBitmapFill*     GetAppliedBitmapColourFill();
	AttrBitmapFill*		GetAppliedBitmapTranspFill();

	// DY, when blending brushes we need to generate the paths and AttrMap ourselves 
	// and then set it to a blendpath.  I don't recommend using these functions under
	// other circumstances.
	void				SetAppliedAttributes(CCAttrMap* pMap);
	void				SetPath(Path* pPath);
	void				DeleteAttributesAndPath();

	// DMc - we sometimes need to set this externally
	void				SetPathType(PathTypeEnum PathType) { m_PathType = PathType; }

	// new transform function also records cumulative transformations
	void                Transform(Matrix* pTransMatrix, BOOL TransformLines, BOOL Tile = FALSE);
	BOOL				TransformBrushAttributesBeforeSave();
	BOOL				TransformBrushAttributesAfterSave();
	Path*				m_pPath;

	void SetBlendNode(NodeRenderableInk *pInk) { m_pBlendNode = pInk; }
	
	void				SetFreeAttributeFlag(BOOL Value);
	void				SetCreatedViaNodeBlendPath (BOOL val)	{ m_bCreatedViaNodeBlendPath = val; }
	BOOL				GetCreatedViaNodeBlendPath ()			{ return (m_bCreatedViaNodeBlendPath); }

	
	// Access the copy data used for brush rendering
	BOOL MakeCopyPathAndAttributes();
	BOOL UpdateCopyPathAndAttributes();
	CCAttrMap* MakeAttributeMapCopy();
	CCAttrMap* UpdateAttributeMapCopy(CCAttrMap* pCopyMap);
	void DeleteCopyPathAndAttributes();
	void DeleteAttributeMapAndAttributes(CCAttrMap* pMap);
	Path* GetCopyPath();
	CCAttrMap* GetCopyAttributes();


private:
	void				ProcessVerbs();
	void 				CorrectDirection(INT32 Index);

	// NodePath			* m_pNodePath;
	NodeRenderableInk*	m_pBlendNode;
	
	NodeRenderableInk*	m_pCreatedByNode;
	CCAttrMap*			m_pAppliedAttrs;

	UINT32				m_CloseFigure;
	INT32				m_OrigMapping;
	PathTypeEnum		m_PathType;
	BOOL				m_DirChanged;
	BOOL				m_UniqueAttrs;

	UINT32				m_SubPathID;
	UINT32				m_SubPathNum;
	INT32				m_NumElements;

	DocCoord			m_BrushStartCoord;  //the starting position of the blendpath when 
											// it is created as part of a brush
	DocCoord            m_BrushCurrentCoord; // used to record the position of the blendpath just 
											 // before it is saved as part of a brush

	BOOL				m_bDeleteAll;      // bodgey flag to indicate that we should delete everything upon destruction
	BOOL				m_bFreeAttrMapOnDestruct; // another bodgey flag to indicate that we should definitely delete 
												  // the attribute map upon destruction

	MILLIPOINT			m_AppliedLineWidth;  // caches the line width applied to this BP

	BOOL				m_bCreatedViaNodeBlendPath;		// CGS:  NEED TO KNOW this now!

	
	// Copies of the path and attributes used for brush rendering
	Path* m_pCopyPath;
	CCAttrMap* m_pCopyAttrs;
};

/***********************************************************************************************

>	class BlendRef : public CC_CLASS_MEMDUMP

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Purpose:	Contains all the data that a NodeBlender needs about another node in the tree
				which it is blending.

***********************************************************************************************/

class BlendRef : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(BlendRef);

public:
	BlendRef();
	~BlendRef();

/*	BOOL 				Initialise(	NodeRenderableInk* pThisNode,
									INT32 Index,
									UndoableOperation* pUndoOp,
									Progress* pProgress,
									BOOL IgnoreEscape,
									Trans2DMatrix* pMatrix 
									);
*/	
	// overridden version for getting paths out of nodes with brush attributes
	BOOL 				Initialise(	NodeRenderableInk* pThisNode,
									INT32 Index,
									UndoableOperation* pUndoOp,
									Progress* pProgress,
									BOOL IgnoreEscape,
									Trans2DMatrix* pMatrix,
									AttrBrushType* pAttrBrush = NULL);

	BOOL				InitialiseForBrush(NodeRenderableInk* pThisNode);

	NodeRenderableInk*	GetNode() { return m_pNode; }
	void 				Transform( TransformBase& Trans );
	void 				RenderBlendBlobs(RenderRegion* pRender,Trans2DMatrix* pMatrix);
	void				PreBlend();
	void				PostBlend();
	CCAttrMap*			FindAppliedAttributes(BlendPath* pBlendPath);

	BOOL		AddBlendPath(BlendPath* pBlendPath);
	UINT32		GetNumBlendPaths()	{ return m_NumBlendPaths; }
	BlendPath*	GetFirstBlendPath();
	BlendPath*	GetNextBlendPath(BlendPath* pCurrentBlendPath);
	BlendPath*	GetBlendPath(INT32 Index);
	BOOL 		IsPointOverBlob(DocCoord* pPointerPos,BlendPath** ppBlendPath,INT32* pIndex,Trans2DMatrix* pMatrix);
	INT32		GetOrigMapping();
	void		SetAWComplex(BOOL state) 	{ m_AWComplex = state; }
	BOOL		GetAWComplex() 				{ return m_AWComplex; }

	DocRect		GetBoundingRect();

	BOOL		RemoveLastBlendPath();
//	static void InitAddNode(BlendRef* pRef);
//	static void DeinitAddNode();
//	static BOOL AddNode(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedBy);

	void StripRedundantNodeBlendPaths (BlendRef* spouse);

protected:
//	static BlendRef* pRefAddNode;	

	NodeRenderableInk*	m_pNode;
	List				m_BlendPathList;
	UINT32				m_NumBlendPaths;
	CMapPtrToPtr*		m_pBlendPathAttrMap;
	BOOL				m_AWComplex;
};


/***********************************************************************************************

>	class NodeBlender : public NodeRenderableInk

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Purpose:	A NodeBlender is the class that actually renders the blend stages of a blend object.
				It also generates NodePath objects when the user does a 'make shapes' on a blend

***********************************************************************************************/

class NodeBlender : public NodeRenderableInk
{
// Give my name out in memory dumps
CC_DECLARE_DYNCREATE(NodeBlender);

public:
	// Default constructor and Destructor
	NodeBlender(); 
	~NodeBlender(); 
	
	NodeBlender(Node* ContextNode,  
				AttachNodeDirection Direction,  
				BOOL Locked = FALSE, 
				BOOL Mangled = FALSE,  
				BOOL Marked = FALSE, 
				BOOL Selected = FALSE    
			  );

	// NodeBlender-specific functions
	BOOL Initialise(NodeRenderableInk* 	pNodeStart,
					NodeRenderableInk* 	pNodeEnd,
					INT32			 	PathIndexStart,
					INT32				PathIndexEnd,
					UndoableOperation* 	pUndoOp,
					Progress*			pProgress,
			 		BOOL 				IgnoreEscape);

	// This sets the Initialised flag to FALSE, and deletes any memory created during initialisation.
	// The ObjIndexStart, ObjIndexEnd and NumBlendSteps remain the same, so the Reinit() func can exactly
	// recreate the blender node.
	// Mainly used to free up some memory when hiding NodeBlender objects in the tree
	void Deinit(BOOL bNodesMayBeChanged = FALSE);

	// If the blender is not initialised (usu. after a call to Deinit()) this call will reinitialise
	// if possible.
	// The two params point to the two nodes the blender should initialise.
	// You can get ptrs to the current nodes that blender is referencing using GetNodeStart() & GetNodeEnd().
	BOOL Reinit(NodeRenderableInk* pInkNodeStart = NULL,NodeRenderableInk* pInkNodeEnd = NULL,BOOL ProgressBar = TRUE);

	// These return to the two nodes tha blender is blending together
	NodeRenderableInk* 	GetNodeStart() 	{ return m_pNodeStart; }
	NodeRenderableInk* 	GetNodeEnd() 	{ return m_pNodeEnd; }	

	// The copy function
	virtual Node* SimpleCopy();

	// virtual Functions to deal with bounding boxes
	virtual DocRect	GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE);
	virtual DocRect GetBlobBoundingRect();
	DocRect GetUnionBlendedBounds(DocRect& BoundsStart,DocRect& BoundsEnd);

	// Rendering Functions
	virtual void Render( RenderRegion* pRender );
	virtual void RenderEorDrag( RenderRegion* );
	virtual void RenderBlendBlobs(RenderRegion* pRender,BOOL RenderStart,BOOL RenderEnd);
	virtual void PreExportRender( RenderRegion* pRender ); 
	virtual BOOL ExportRender( RenderRegion* pRender ); 

	// Export-related funcs
	BOOL IsArtWorksEPSCompatible();
	BOOL IsComplex();
	INT32 GetAWStartPathIndex();
	INT32 GetAWEndPathIndex();
	INT32 GetAWPathIndex(BlendRef* pRef,INT32 PathIndex);

	// Functions to transform the shape
	virtual void Transform( TransformBase& );

	// Utility funcs 
	BOOL IsPointOverBlob(DocCoord* pPointerPos,BlendPath** ppBlendPath,INT32* pIndex,BOOL* pAStart);
	BOOL Remap(DocCoord PosStart,DocCoord PosEnd,DocCoord * pInvPosStart,DocCoord * pInvPosEnd);
	BOOL ConvertAWPathIndexesToCamelot(INT32 *pPathIndexStart,INT32* pPathIndexEnd);
	BOOL FindPathEndPoint(Node* pNodePath,INT32* pIndex);
	NodeRenderableInk* FindObjIndexedNode(INT32 ObjIndex);
	virtual BOOL CanSelectAsSimple();

	// Returns the NodeBlend that this blender belongs to.
	NodeBlend* GetNodeBlend();

	// The OneToOne flag (asks parent NodeBlend node)
	BOOL IsOneToOne();

	// The NotAntialiased flag - TRUE when we don't want the blend steps to antialiased (asks parent NodeBlend node)
	BOOL IsNotAntialiased();

	// Information functions
	virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes
 
	// Make Shapes functions
	virtual BOOL CanBecomeA(BecomeA* pBecomeA);
	virtual BOOL DoBecomeA(BecomeA* pBecomeA);

	// Debugging function
	#ifdef _DEBUG
	void	ShowDebugTreeDetails() const;
	#endif
	void GetDebugDetails( StringBase* Str );
	void GetDebugDetails( StringBase* Str,BlendRef* pBlendRef );

	// Functions to deal with messages
	virtual BOOL OnClick( DocCoord, ClickType, ClickModifiers, Spread* );
	virtual BOOL HidingNode();
	virtual BOOL ShowingNode();
 
	// The natty Snap functions
	virtual BOOL Snap(DocCoord* pDocCoord);
	virtual BOOL Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord);

	// This extracts the num blend steps from the parent blend node
	UINT32 GetNumBlendSteps();

	// Find out how to blend colours by calling the same func in the parent blend node
	ColourBlendType GetColourBlendType();

	// Allows the path start and end indexes to be manipulated (only use if you know what you're doing)
	void SetPathIndexStart(INT32 Index) 	{ m_PathIndexStart = Index; }
	void SetPathIndexEnd  (INT32 Index) 	{ m_PathIndexEnd   = Index; }
	INT32 GetPathIndexStart() 			{ return m_PathIndexStart; }
	INT32 GetPathIndexEnd() 				{ return m_PathIndexEnd; }
	
	// Allows the start and end indexes referencing the two objects this blender is blending to be set and retreived
	//	(only use if you know what you're doing)
	void SetObjIndexStart(INT32 ObjIndex) 	{ m_ObjIndexStart = ObjIndex; }
	void SetObjIndexEnd  (INT32 ObjIndex) 	{ m_ObjIndexEnd   = ObjIndex; }
	INT32 GetObjIndexStart() 				{ return m_ObjIndexStart; }
	INT32 GetObjIndexEnd() 					{ return m_ObjIndexEnd; }
	
	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual BOOL WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter);
	virtual BOOL WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WriteEndChildRecordsNative(BaseCamelotFilter* pFilter);

	// Override this to ensure that bounds never get written out for this node
	virtual BOOL WriteBoundsRecord(BaseCamelotFilter* pFilter) { return(TRUE); }

	// Blend on a path functions
	BOOL			GetPointOnNodeBlendPath(double BlendRatio,DocCoord* pPoint,double* pAngle);
	BOOL			GetPointFromDistance(double Distance, DocCoord* pPoint, double* pAngle);
	NodeBlendPath*	GetNodeBlendPath();
	Trans2DMatrix*	GetRotateMatrix(NodeRenderableBounded* pNode,double Angle);

	double	GetProportionOfPathDistStart();
	double	GetProportionOfPathDistEnd();
	void	SetProportionOfPathDistStart(double p);
	void	SetProportionOfPathDistEnd(double p);

	double	GetAngleStart();
	double	GetAngleEnd();
	void	SetAngleStart(double Angle);
	void	SetAngleEnd(double Angle);
	void	RotateBounds(DocRect& Bounds,double Angle);
	

	//Diccon 26/9/99
	double	GetLinearDistance();  // returns the distance between the start and end nodes
	BOOL	GetBlendDirection(double* Gradient);
	BOOL    GetBlendObjectCentres(DocCoord* FirstCentre, DocCoord* LastCentre);
	
	BOOL	IsTangential();
	void	SetUninitialised() { m_Initialised = FALSE;}

	void	SetBlendedOnCurve(BOOL value)	{ m_BlendedOnCurve = value;}
	BOOL	IsBlendedOnCurve()				{ return m_BlendedOnCurve;}

	// Karim 01/02/2000		Mutate/access methods for m_Reversed.
	//						NB - this can ONLY be set if we're already blended on a curve.
	void	SetReversed(BOOL value)		{ m_Reversed = m_BlendedOnCurve ? value : FALSE; }
	BOOL	IsReversed()				{ return m_Reversed; }

	void	SetNodeBlendPathIndex(INT32 Index);
	INT32    GetNodeBlendPathIndex();

	// reverses the end objects of the blender
	BOOL	ReverseEnds();

	// DMc moved - since NodePath need these now
	static BOOL ConvertLineToShape(Path* pPath,Path* pShapePath);

	// Path processing functions
	static BOOL ProcessBlendedPath(DocCoord* pCoords,PathVerb* pVerbs,PathFlags* pFlags,UINT32 Len,BOOL Filled);
	static void SetPathFlags(PathVerb* pVerbs,PathFlags* pFlags,UINT32 Len);
	static void ReversePath(DocCoord* Coords,PathVerb* Verbs,UINT32 Len);

	void UpdateBlendStartAngles (Trans2DMatrix& trans);

	virtual INT32 EstimateNodeComplexity (OpParam* details);

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	// ResetVars() is called by all constructors to centralise member var initialisation
	void ResetVars();

	// Standard copy function
	void CopyNodeContents(NodeBlender* NodeCopy);

	// Other private functions.	   
	BOOL 				InitBlendRef(	NodeRenderableInk* pNode,
										BlendRef** ppRef,
										INT32 Index,
										UndoableOperation* pUndoOp,
										Progress* pProgress,
										BOOL IgnoreEscape,
										Trans2DMatrix* pMatrix);
	BOOL 				CalcObjIndex(NodeRenderableInk* pInkNode,INT32* pObjIndex);
	void 				CheckFullyInitialised();
	BOOL 				BlendPaths(BlendPath* pBlendPathStart,BlendPath* pBlendPathEnd,double BlendRatio);
	void 				CreateBlends(RenderRegion* pRender,HandleBecomeA* pHandleBecomeA);
	Matrix 				MakeMatrix(BlendPath* pBlendPathStart,BlendPath* pBlendPathEnd,double BlendRatio);
	BOOL 				CheckBoundsIntersect(RenderRegion* pRender,DocRect& BoundsStart,DocRect& BoundsEnd,double BlendRatio);
	DocRect				BlendBounds(DocRect& BoundsStart,DocRect& BoundsEnd,double BlendRatio,BOOL UseBlendPath);

	// Attribute funcs
	BOOL BlendAttributes(RenderRegion *pRender, BlendPath* pBlendPathStart,
						 BlendPath* pBlendPathEnd,CCAttrMap* pBlendedAttribMap,double BlendRatio,
						 double objectRatio, BOOL objectProfileProcessing);
 	//void DeleteAttributes(CCAttrMap* pAttribMap);
	void RenderAttributes(RenderRegion* pRender,CCAttrMap* pAttribMap);

	// Pre and post blend funcs
	BOOL 		PreBlend();
	void 		PostBlend();
	BOOL 		ReallocTempBuffers(UINT32 Size);
	void 		DeallocTempBuffers();

	// Access funcs for the path arrays
	DocCoord*  	GetCoordArray(UINT32 MinSize);
	PathVerb*  	GetVerbArray(UINT32 MinSize);
	PathFlags* 	GetFlagArray(UINT32 MinSize);
	UINT32*     	GetGBlendBuff(UINT32 MinSize);

	// Profile functions
	double MapObjectBlendRatio(double BlendRatio);
	double MapAttrBlendRatio(double BlendRatio);
	double MapPositionBlendRatio(double BlendRatio);

	// bodge functions to deal with transparencies and fill profile problem
	double GetObjectRatio();  
	double GetInvertedAttributeRatio();

	// These two vars point to reference data for the start and end objects that this blender
	// is interested in.
	BlendRef *m_pRefStart;
	BlendRef *m_pRefEnd;

	// These are the to ink nodes that we are blending
	// They are set in initialisation
	NodeRenderableInk* m_pNodeStart;
	NodeRenderableInk* m_pNodeEnd;

	// These two vars hold a value that is relative to the parent node (NodeBlend) 
	// indicating which object the blend references.
	// E.g. If ObjIndexStart = 2, then the start object that this blender will blend is the third NodeRenderableInk
	// node (starting from the first child) under the parent NodeBlend node.
	// This allows use to re-initialise the blender after a copy has been made (via SimpleCopy).
	// When any public function of NodeBlender is called (e.g. Render(), Transform() etc), Reinit() should be 
	// the first thing the func should do, just in case the blender has to reinit itself.
	INT32 m_ObjIndexStart;
	INT32 m_ObjIndexEnd;

	// This will remain FALSE until the blender is fully initialised, i.e. after both pRefStart
	// and pRefEnd point to filled-in BlendRef structs, and ObjIndexStart and ObjIndexEnd are >=0
	// (i.e. they reference the nodes stored in pRefStart and pRefEnd in a relative manner)
	BOOL m_Initialised;

	// This is the current blend step to render. This will allow interruptable blend rendering, when we
	// get round to doing it.
	UINT32 m_BlendStep;

	// The following set of vars deal with arrays that are set up by the PreBlend() routine
	// Also, these arrays can also change by calls to the access funcs GetCoordArray(), GetVerbArray() and GetFlagArray()
	UINT32 		m_TempArraySize;
	DocCoord*	m_pTempCoords;
	PathVerb*	m_pTempVerbs;
	PathFlags* 	m_pTempFlags;
	UINT32		m_GBlendBuffSize;
	UINT32*		m_pGBlendBuff;
	UINT32		m_ArrayLength;

	// These hold the indexes of the path elements that reference the path blobs the user dragged
	// on when generating the blend.  These will only be != -1 when blending from a path to a path,
	// and both paths are selected and the user started the drag on one blob and ended it on another.
	INT32		m_PathIndexStart;
	INT32		m_PathIndexEnd;

	// These hold the proportion of the distance of the path where the start and end objects appear.
	// e.g. values of 0.2 and 0.5 would mean this blender starts a 5th of the way along the path,
	// and ends half way along the path.
	// Used to calculate where blended paths are positioned.
	double		m_ProportionOfPathDistStart;
	double		m_ProportionOfPathDistEnd;

	double		m_AngleStart;
	double		m_AngleEnd;

	// Diccon 9/99 Identifies whether this blend is between a blend on a curve and
	// another object.
	BOOL	m_BlendedOnCurve;
	BOOL    m_Reversed;
	
	// if we are on a path then this identifies which one (-1 means none)
	INT32    m_NodeBlendPathIndex;

	BOOL CallBeginBlendStep(BlendNodeParam * pParam, BOOL* usingSumAllPathsPathProcessor);
	BOOL CallEndBlendStep(BlendNodeParam * pParam);

}; 


/***********************************************************************************************

>	class ListStepper

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Purpose:	This class it used to step along two lists, choosing matching pairs.
				This really comes into play when you have different numbers of items in the two
				lists, and you want to step along them without missing out any items.
				E.g. List A contains 4 items, list B contains 2 items
					this would produce the following pairs:
						A	B
						0	0
						1	0
						2	1
						3	1
				E.g. List A contains 3 List B contains 5
						A	B
						0	0
						0	1
						1	2
						1	3
						2	4

***********************************************************************************************/

class ListStepper
{
public:
	ListStepper() { Min = Max = NextInStart = NextInEnd = Inc = 0; pNextInMin = pNextInMax = NULL;}

	void Init(UINT32 NumInStart,UINT32 NumInEnd,UINT32 Start);
 	void GetNext(INT32* pNextInStart,INT32* pNextInEnd);

private:
	UINT32 Min;
	UINT32 Max;
	UINT32 NextInStart;
	UINT32 NextInEnd;
	UINT32 Inc;
	UINT32* pNextInMin;
	UINT32* pNextInMax;
};

/***********************************************************************************************

>	class HandleBecomeA: public CC_CLASS_MEMDUMP

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/94
	Purpose:	Objects of this class are passed into NodeBlender::CreateBlends() so that they can
				receive all the paths the blender generates in order to convert the blender into
				shapes (or more specifically NodePaths)

***********************************************************************************************/

class HandleBecomeA : public CC_CLASS_MEMDUMP
{
CC_DECLARE_MEMDUMP(HandleBecomeA);
public:
	HandleBecomeA(	NodeBlender* 	pThisNodeBlender,
					Node* 			pThisContextNode,
					BecomeA* 		pThisBecomeA) : 
					pNodeBlender(pThisNodeBlender), 
					pBecomeA(pThisBecomeA),
					pContextNode(pThisContextNode),
					pNodeGroup(NULL),
					LastStep(0),
					NumPathsPerStep(0) {}

	BOOL PassBack(Path* pBlendedPath,CCAttrMap* pBlendedAttrMap,UINT32 Step);

	void SetNumPathsPerStep(UINT32 Num)	{ NumPathsPerStep = Num; }
	BOOL MakeGroupPerStep()				{ return (NumPathsPerStep > 1); }

	BecomeA* GetBecomeA () { return (pBecomeA); }
	Node*	 GetContextNode () { return (pContextNode); }

private:
	NodeBlender* pNodeBlender;
	BecomeA*	 pBecomeA;
	Node*		 pContextNode;
	NodeGroup*	 pNodeGroup;
	UINT32 		 LastStep;
	UINT32		 NumPathsPerStep;
};

class NodeCompoundBlendBecomeA : public BecomeA
{
CC_DECLARE_MEMDUMP(NodeCompoundBlendBecomeA);
public:
	NodeCompoundBlendBecomeA(	BecomeAReason Reason,
								CCRuntimeClass* pClass, 
								UndoableOperation* pOp, 
								BOOL Sel,
								Node* pContext
							) :

				BecomeA (Reason,pClass,pOp, Sel)
				{
					pContextNode = pContext;
					pPathsToBecome = NULL;
				}

	// This function should be called when Reason == BECOMEA_PASSBACK 
	//virtual BOOL PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap);

	Node* GetContextNode () { return (pContextNode); }
	List* GetPathsToBecome () { return (pPathsToBecome); }

	void SetPathsToBecome (List* pathsList) { pPathsToBecome = pathsList; } //SumAllPathsElem* become) { pPathToBecome = become; }

private:
	Node* pContextNode;			// the context node that we are to attach to
	List* pPathsToBecome;		// the node that we are to become
};

//-----------------------------------------------------------
/***********************************************************************************************

>	class BlendBecomeA: public BecomeA

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/94
	Purpose:	This is the class that is passed to other nodes when blenders try and get them to 
				to turn into other types via their DoBecomeA() method

***********************************************************************************************/

class BlendBecomeA : public BecomeA
{
CC_DECLARE_MEMDUMP(BlendBecomeA);
public:
	BlendBecomeA(	BecomeAReason Reason,
					CCRuntimeClass* pClass, 
					UndoableOperation* pOp, 
					BlendRef* pThisBlendRef, 
					INT32 ThisIndex,
					Progress* pThisProgress,
					BOOL IgnoreEsc,
					Trans2DMatrix* pMatrix) :

				BecomeA(Reason,pClass,pOp), 
				pBlendRef(pThisBlendRef), 
				Index(ThisIndex), 
				SubPathID(0), 
				pProgress(pThisProgress),
				IgnoreEscape(IgnoreEsc),
				m_pMatrix(pMatrix)		{ } ;

	BOOL IsBlendBecomeA() { return TRUE; }

	// This function should be called when Reason == BECOMEA_PASSBACK 
	virtual BOOL PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap);

private:
	BlendRef*		pBlendRef;
	INT32			Index;
	UINT32			SubPathID;
	Progress*		pProgress;
	BOOL 			IgnoreEscape;
	Trans2DMatrix*	m_pMatrix;

	NodeRenderableInk * CreateWrapNode(NodeRenderableInk * pNode, NodeCompound *pTree,
														CCAttrMap * pMap);
};

#endif	// INC_NODEBLDR
