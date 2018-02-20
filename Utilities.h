#include <wrl.h>

///<summary>Helper function: Allows DirectX APIs to work with exceptions</summary>
inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		// Set a breakpoint on this line to catch Win32 API errors.
		throw Platform::Exception::CreateException(hr);
	}
}

