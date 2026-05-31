#pragma once

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass& input);
	~InputClass();

	void Initialize();

	void KeyDown(unsigned int input);
	void KeyUp(unsigned int input);

	bool IsKeyDown(unsigned int key);

private:
	bool m_keys[256];
};