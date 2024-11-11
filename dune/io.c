/*
* raw(?) I/O
*/
#include <windows.h>
#include "io.h"

void gotoxy(POSITION pos) {
	COORD coord = { pos.column, pos.row }; // ��, �� �ݴ�� ����
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void set_color(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void printc(POSITION pos, char ch, int color) {
	if (color >= 0) {
		set_color(color);
	}
	gotoxy(pos);
	printf("%c", ch);
}

KEY get_key(void) {
	static KEY last_key = k_none;            // ���������� �Էµ� Ű ����
	static int last_key_press_time = 0;      // ������ Ű �Է� �ð��� ���
	int current_time = sys_clock;            // ���� �ð�

	if (!_kbhit()) {  // �Էµ� Ű�� �ִ��� Ȯ��
		return k_none;
	}

	int byte = _getch();    // �Էµ� Ű�� ���� �ޱ�

	switch (byte) {
	case 'q': return k_quit;  // 'q'�� ������ ����
	case ' ': return k_space; //space�Է�
	case 27: return k_escape; // ESC �Է� ó��
	case 224:
		byte = _getch();  // MSB 224�� �Է� �Ǹ� 1����Ʈ �� ���� �ޱ�
		switch (byte) {
			case 72: return k_up;
			case 75: return k_left;
			case 77: return k_right;
			case 80: return k_down;
			default: return k_undef;
		}
	default: return k_undef;
	}
}

void set_cursor_position(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void clear_line_from_cursor() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD length, written;
	COORD cursor_pos;

	// �ܼ� �ڵ� ��������
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// ���� Ŀ�� ��ġ�� �ܼ� ���� ��������
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	cursor_pos = csbi.dwCursorPosition;

	// ���� Ŀ�� ��ġ���� �� �������� ���� ���
	length = csbi.dwSize.X - cursor_pos.X;

	// �������� ä���� �� ������ �����
	FillConsoleOutputCharacter(hConsole, ' ', length, cursor_pos, &written);
	SetConsoleCursorPosition(hConsole, cursor_pos);  // Ŀ�� ��ġ ����
}

