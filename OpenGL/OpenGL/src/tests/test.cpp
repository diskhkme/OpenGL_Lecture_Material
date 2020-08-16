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
			//string���� ��ư�� �����ϰ�
			if (ImGui::Button(test.first.c_str()))
			{
				//������ current�� �ش� functional�� �Ҵ�
				m_CurrentTest = test.second();
			}
		}
	}
}