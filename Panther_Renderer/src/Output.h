#pragma once

namespace Panther
{
	class Output
	{
	public:
		Output(IDXGIOutput4* a_Output);
		~Output() {};

	private:
		Output(const Output&) = delete;

		const Microsoft::WRL::ComPtr<IDXGIOutput4> m_Output;
	};
}