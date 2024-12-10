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
void display_command_title();
void display_commands(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], CURSOR cursor, int is_update_requested, int reset);
void display_systemMessage_title();
void display_system_message(int is_update_requested, int reset);

void add_system_message(const char* message);
void add_system_message_2(const char* message);

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
	display_systemMessage_title();
	display_system_message(is_update_requested, reset);
	display_object_info(map, cursor, is_update_requested, reset);
	display_command_title();
	display_commands(map, cursor, is_update_requested, reset);
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

void display_command_title() {
	set_color(15);  // ������Ʈ ���� ���� ����
	set_cursor_position(MAP_WIDTH + 1, MAP_HEIGHT+1);
	printf("==========���â==========\n");
}

char system_messages_2[50][50]; // �޽��� �迭
int message_count_2 = 0;

void display_commands(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], CURSOR cursor, int is_update_requested, int reset) {
	// esc�� ������ �� ����â �ʱ�ȭ

	if (reset == 1) {
		set_cursor_position(MAP_WIDTH + 2, MAP_HEIGHT+2);
		add_system_message_2(" ");
	}

	//space�� ������ �� �ش� ��ɾ� ���
	if (is_update_requested == 1) {
		set_color(15);  // ��ɾ� ���� ����
		int current_info_y = MAP_HEIGHT+2;  // ����â ����� Y�� ���� ��ġ
		int layer;
		char object_repr = ' ';  // �⺻������ �� �������� ����


		// ���� Ŀ�� ��ġ���� `map`�� `repr` �� ��������
		for (int layer = 0; layer < N_LAYER; layer++) {
			object_repr = map[layer][cursor.current.row][cursor.current.column];

			if (object_repr != ' ') {
				break;
			}
		};

		// ����â���� ��ɾ� ��� ��ġ ����
		set_cursor_position(MAP_WIDTH + 1, current_info_y);

		//clear_line_from_cursor();

		switch (object_repr) {
		case 'B':
			if ((cursor.current.row <= 2 && cursor.current.column >= MAP_WIDTH - 5) || (cursor.current.row >= 15 && cursor.current.column <= 2)) {
				add_system_message_2("H: �Ϻ����� ����");
				//printf("H: �Ϻ����� ����");
			}
			else {
				add_system_message_2("S: ��������");
				//printf("S: ��������");
			}
			break;
		case 'P' || 'D' || 'G' || 'R':
			add_system_message_2(" ");
			//printf(" ");
			break;
		case 'S':
			if ((cursor.current.row == 7 && cursor.current.column == 58) || (cursor.current.row == 9 && cursor.current.column == 1)) {
				add_system_message_2(" ");
				//printf(" ");
			}
			else {
				add_system_message_2("F: ������ ����");
				//printf("F: ������ ����");
			}
			break;
		default:
			add_system_message_2(" ");
			//printf(" ");
			break;
		}

		// current_info_y�� ���� ������ ���� �ʵ��� �ʱ�ȭ (��: ����â ���� ����)
		if (current_info_y > MAP_HEIGHT) { // ����â ��� ���� ���� ����
			current_info_y = 2;
		}
	}
}

void display_systemMessage_title() {
	set_color(15);  // ������Ʈ ���� ���� ����
	set_cursor_position(0, MAP_HEIGHT + 1);
	printf("==========�ý��۸޼���==========");
}


// �ý��� �޽��� ����
#define MESSAGE_LOG_SIZE 5 // �޽��� �α� ũ�� ����
char system_messages[MESSAGE_LOG_SIZE][50]; // �޽��� �迭
int message_count = 0; // ���� ����� �޽��� ��

void display_system_message(int is_update_requested, int reset) {
	// ���� üũ �� �޽��� �߰�
	if (is_update_requested == 1) {
		add_system_message("�ǹ�/������ �����߽��ϴ�.");
	}

	if (reset == 1) {
		add_system_message("����â�� ��ɾ �ʱ�ȭ�մϴ�.");
	}
	
		// �޽����� ȭ�鿡 ���
	for (int i = 0; i < message_count; i++) {
		set_cursor_position(0, MAP_HEIGHT + 2 + i); // �޽��� ��� ��ġ ����
		clear_line_from_cursor(); // ���� �޽����� ����
		printf("%s", system_messages[i]); // �޽��� ���
		for (int j = 0; j < 1; j++) {
			set_cursor_position(MAP_WIDTH + 2, MAP_HEIGHT + 2);
			printf("%s", system_messages_2[message_count_2 - 1]); // �޽��� ���
		}
	}
}

// ���ο� �޽����� �߰��ϴ� �Լ�
void add_system_message(const char* message) {
	if (message_count < MESSAGE_LOG_SIZE) {
		// �迭�� ���� ���� �ʾ����� �޽��� �߰�
		snprintf(system_messages[message_count], 50, "%s", message);
		message_count++;
	}
	else {
		// �迭�� ���� á���� ��ũ�� ����
		for (int i = 0; i < MESSAGE_LOG_SIZE - 1; i++) {
			strncpy_s(system_messages[i], sizeof(system_messages[i]), system_messages[i + 1], sizeof(system_messages[i + 1]));
		}
		// ������ �ٿ� ���ο� �޽����� �߰�
		snprintf(system_messages[MESSAGE_LOG_SIZE - 1], 50, "%s", message);
	}
}

void add_system_message_2(const char* message) {
	snprintf(system_messages_2[message_count_2], 50, "%s", message);
	message_count_2++;
}