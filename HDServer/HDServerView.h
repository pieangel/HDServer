
// HDServerView.h : interface of the CHDServerView class
//

#pragma once


class CHDServerView : public CView
{
protected: // create from serialization only
	CHDServerView() noexcept;
	DECLARE_DYNCREATE(CHDServerView)

// Attributes
public:
	CHDServerDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CHDServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in HDServerView.cpp
inline CHDServerDoc* CHDServerView::GetDocument() const
   { return reinterpret_cast<CHDServerDoc*>(m_pDocument); }
#endif

