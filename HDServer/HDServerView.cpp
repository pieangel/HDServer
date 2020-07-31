
// HDServerView.cpp : implementation of the CHDServerView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "HDServer.h"
#endif

#include "HDServerDoc.h"
#include "HDServerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHDServerView

IMPLEMENT_DYNCREATE(CHDServerView, CView)

BEGIN_MESSAGE_MAP(CHDServerView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CHDServerView construction/destruction

CHDServerView::CHDServerView() noexcept
{
	// TODO: add construction code here

}

CHDServerView::~CHDServerView()
{
}

BOOL CHDServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CHDServerView drawing

void CHDServerView::OnDraw(CDC* /*pDC*/)
{
	CHDServerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CHDServerView printing

BOOL CHDServerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CHDServerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CHDServerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CHDServerView diagnostics

#ifdef _DEBUG
void CHDServerView::AssertValid() const
{
	CView::AssertValid();
}

void CHDServerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CHDServerDoc* CHDServerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CHDServerDoc)));
	return (CHDServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CHDServerView message handlers
