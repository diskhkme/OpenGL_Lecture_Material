#include "test.h"
#include "imgui/imgui.h"

namespace test
{
	TestMenu::TestMenu(Test*& currrentTestPointer)
		: m_CurrentTest{ currrentTestPointer }
	{

	}

	void TestMenu::OnImGuiRender()
	{
		for (auto& test : m_Tests)
		{
			//string으로 버튼을 생성하고
			if (ImGui::Button(test.first.c_str()))
			{
				//눌리면 current를 해당 functional로 할당
				m_CurrentTest = test.second();
			}
		}
	}
}