#pragma once

class BranchTrampoline
{
public:
	BranchTrampoline();
	virtual ~BranchTrampoline();

	virtual bool Create(size_t len, void * module = NULL);
	void Destroy();

	// allocate unsized 
	virtual bool IsValid() const;
	virtual void * StartAlloc();
	virtual void EndAlloc(const void * end);

	virtual void * Allocate(size_t size = sizeof(void *));

	virtual size_t Remain()	{ return m_len - m_allocated; }

	// takes 6 bytes of space at src, 8 bytes in trampoline
	virtual bool Write6Branch(uintptr_t src, uintptr_t dst);
	virtual bool Write6Call(uintptr_t src, uintptr_t dst);

	// takes 5 bytes of space at src, 14 bytes in trampoline
	virtual bool Write5Branch(uintptr_t src, uintptr_t dst);
	virtual bool Write5Call(uintptr_t src, uintptr_t dst);

private:
	// takes 6 bytes of space at src, 8 bytes in trampoline
	bool Write6Branch_Internal(uintptr_t src, uintptr_t dst, UInt8 op);

	// takes 5 bytes of space at src, 14 bytes in trampoline
	bool Write5Branch_Internal(uintptr_t src, uintptr_t dst, UInt8 op);

	void	* m_base;
	size_t	m_len;			// bytes
	size_t	m_allocated;	// bytes

	void	* m_curAlloc;	// currently active StartAlloc base
};

extern BranchTrampoline g_branchTrampoline;
extern BranchTrampoline g_localTrampoline;

BranchTrampoline& GetBranchTrampoline_Internal();
BranchTrampoline& GetLocalTrampoline_Internal();