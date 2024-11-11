/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#include "display.h"
#include "io.h"

// ����� ������� �»��(topleft) ��ǥ
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };

char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
void display_status_title();
void display_object_info(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], CURSOR cursor, int is_update_requested, int reset);

void display(   //�ڿ���, 
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor,
	int is_update_requested,
	int reset)
{
	display_resource(resource);
	display_status_title();
	display_map(map);
	display_cursor(cursor);
	// display_system_message()
	display_object_info(map, cursor, is_update_requested, reset);
	// display_commands()
	// ...
}

void display_resource(RESOURCE resource) {
	set_color(COLOR_RESOURCE);
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}

// subfunction of draw_map()
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] >= 0) {
					dest[i][j] = src[k][i][j];
				}
			}
		}
	}
}

// ���� ��ȯ �Լ�
int get_color(char tile, int row, int col) {
	switch (tile) {
	case 'B':
		if (row <= 2 && col >= MAP_WIDTH - 5) {
			return 64;  // ���ڳ� ���� (������),
		}
		else {
			return 144;  // ��Ʈ���̵� ���� (�Ķ���)
		}
	case 'P': return 128;    // ����
	case 'W': return 224;    // �����
	case 'R': return 112;    // ����
	case 'S': return 96;     // �����̽�
	case 'H':
		if (row == 3 && col >= MAP_WIDTH - 3 && col <= MAP_WIDTH - 2) {
			return 64;  // ���ڳ� �Ϻ�����
		}
		else {
			return 144;  //��Ʈ���̵� �Ϻ�����
		}
	default: return COLOR_DEFAULT;
	}
}

void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf); // ���� �� ���¸� backbuf�� ����

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {    // backbuf�� frontbuf�� �ٸ� ���� ����
				POSITION pos = { i, j };
				int color = get_color(backbuf[i][j], i, j);  // �ش� Ÿ���� ���� ���
				printc(padd(map_pos, pos), backbuf[i][j], color);
			}
			frontbuf[i][j] = backbuf[i][j];   // backbuf�� �ֽ� ������ frontbuf�� ����
		}
	}
}

void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	// ���� Ŀ�� ��ġ�� ������Ʈ�� ���� �������� ����
	char prev_ch = frontbuf[prev.row][prev.column];
	int prev_color = get_color(backbuf[prev.row][prev.column], prev.row, prev.column);
	printc(padd(map_pos, prev), prev_ch, prev_color);

	// ���� Ŀ�� ��ġ�� Ŀ�� �������� ǥ��
	char curr_ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), curr_ch, COLOR_CURSOR);
}

void display_status_title() {
	set_color(15);  // ������Ʈ ���� ���� ����
	set_cursor_position(MAP_WIDTH + 1, 0);
	printf("==========����â==========\n");
}

void display_object_info(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], CURSOR cursor, int is_update_requested, int reset) {
	// esc�� ������ �� ����â �ʱ�ȭ
	if (reset == 1) {
		set_cursor_position(MAP_WIDTH + 1, 1);
		clear_line_from_cursor();
	}

	//space�� ������ �� �ش� ������Ʈ ���� ���
	if (is_update_requested == 1) {
		set_color(15);  // ������Ʈ ���� ���� ����
		int current_info_y = 1;  // ����â ����� Y�� ���� ��ġ (�ʱⰪ 1)
		int layer;
		char object_repr = ' ';  // �⺻������ �� �������� ����


		// ���� Ŀ�� ��ġ���� `map`�� `repr` �� ��������
		for (int layer = 0; layer < N_LAYER; layer++) {
			object_repr = map[layer][cursor.current.row][cursor.current.column];

			if (object_repr != ' ') {
				break;
			}
		};

		// ����â���� ������Ʈ ���� ��� ��ġ ����
		set_cursor_position(MAP_WIDTH + 1, current_info_y);

		clear_line_from_cursor();

			switch (object_repr) {
			case 'B':
				if (cursor.current.row <= 2 && cursor.current.column >= MAP_WIDTH - 5) {
					printf("����: ���ڳ� ����");
				}
				else {
					printf("����: ��Ʈ���̵� ����");
				}
				break;
			case 'H':
				if (cursor.current.row == 3 && cursor.current.column >= MAP_WIDTH - 3 && cursor.current.column <= MAP_WIDTH - 2) {
					printf("�Ϻ�����: ���ڳ� �ڿ� ä�� ����");  // ���ڳ� �Ϻ�����
				}
				else {
					printf("�Ϻ�����: ��Ʈ���̵� �ڿ� ä�� ����");  // ��Ʈ���̵� �Ϻ�����
				}
				break;
			case 'W':
				printf("�����: �縷�� ������");
				break;
			case 'S':
				printf("�����̽� ������: �ڿ� ����");
				break;
			case 'P':
				printf("����: �Ǽ� ���� ����");
				break;
			case 'R':
				printf("����: �̵� �Ұ� ����");
				break;
				// �߰����� ������Ʈ�� ���� ���̽� �ۼ� ����
			default:
				printf("�縷");
				break;
			}

			printf("\n");

			// current_info_y�� ���� ������ ���� �ʵ��� �ʱ�ȭ (��: ����â ���� ����)
			if (current_info_y > MAP_HEIGHT) { // ����â ��� ���� ���� ����
				current_info_y = 2;
			}
		
	}
}