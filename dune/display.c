/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"

// 출력할 내용들의 좌상단(topleft) 좌표
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

void display(   //자원량, 
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

// 색상 반환 함수
int get_color(char tile, int row, int col) {
	switch (tile) {
	case 'B':
		if (row <= 2 && col >= MAP_WIDTH - 5) {
			return 64;  // 하코넨 본진 (빨간색),
		}
		else {
			return 144;  // 아트레이디스 본진 (파란색)
		}
	case 'P': return 128;    // 장판
	case 'W': return 224;    // 샌드웜
	case 'R': return 112;    // 바위
	case 'S': return 96;     // 스파이스
	case 'H':
		if (row == 3 && col >= MAP_WIDTH - 3 && col <= MAP_WIDTH - 2) {
			return 64;  // 하코넨 하베스터
		}
		else {
			return 144;  //아트레이디스 하베스터
		}
	default: return COLOR_DEFAULT;
	}
}

void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf); // 현재 맵 상태를 backbuf에 복사

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {    // backbuf와 frontbuf가 다를 때만 갱신
				POSITION pos = { i, j };
				int color = get_color(backbuf[i][j], i, j);  // 해당 타일의 색상 얻기
				printc(padd(map_pos, pos), backbuf[i][j], color);
			}
			frontbuf[i][j] = backbuf[i][j];   // backbuf의 최신 정보를 frontbuf로 복사
		}
	}
}

void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	// 이전 커서 위치의 오브젝트를 원래 색상으로 복구
	char prev_ch = frontbuf[prev.row][prev.column];
	int prev_color = get_color(backbuf[prev.row][prev.column], prev.row, prev.column);
	printc(padd(map_pos, prev), prev_ch, prev_color);

	// 현재 커서 위치를 커서 색상으로 표시
	char curr_ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), curr_ch, COLOR_CURSOR);
}

void display_status_title() {
	set_color(15);  // 오브젝트 정보 색상 설정
	set_cursor_position(MAP_WIDTH + 1, 0);
	printf("==========상태창==========\n");
}

void display_object_info(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], CURSOR cursor, int is_update_requested, int reset) {
	// esc를 눌렀을 떄 상태창 초기화
	if (reset == 1) {
		set_cursor_position(MAP_WIDTH + 1, 1);
		clear_line_from_cursor();
	}

	//space를 눌렀을 때 해당 오브젝트 상태 출력
	if (is_update_requested == 1) {
		set_color(15);  // 오브젝트 정보 색상 설정
		int current_info_y = 1;  // 상태창 출력의 Y축 시작 위치 (초기값 1)
		int layer;
		char object_repr = ' ';  // 기본값으로 빈 공간으로 설정


		// 현재 커서 위치에서 `map`의 `repr` 값 가져오기
		for (int layer = 0; layer < N_LAYER; layer++) {
			object_repr = map[layer][cursor.current.row][cursor.current.column];

			if (object_repr != ' ') {
				break;
			}
		};

		// 상태창에서 오브젝트 정보 출력 위치 설정
		set_cursor_position(MAP_WIDTH + 1, current_info_y);

		clear_line_from_cursor();

			switch (object_repr) {
			case 'B':
				if (cursor.current.row <= 2 && cursor.current.column >= MAP_WIDTH - 5) {
					printf("본진: 하코넨 본부");
				}
				else {
					printf("본진: 아트레이디스 본부");
				}
				break;
			case 'H':
				if (cursor.current.row == 3 && cursor.current.column >= MAP_WIDTH - 3 && cursor.current.column <= MAP_WIDTH - 2) {
					printf("하베스터: 하코넨 자원 채취 유닛");  // 하코넨 하베스터
				}
				else {
					printf("하베스터: 아트레이디스 자원 채취 유닛");  // 아트레이디스 하베스터
				}
				break;
			case 'W':
				printf("샌드웜: 사막의 포식자");
				break;
			case 'S':
				printf("스파이스 매장지: 자원 보유");
				break;
			case 'P':
				printf("장판: 건설 가능 지형");
				break;
			case 'R':
				printf("바위: 이동 불가 지형");
				break;
				// 추가적인 오브젝트에 대한 케이스 작성 가능
			default:
				printf("사막");
				break;
			}

			printf("\n");

			// current_info_y가 일정 범위를 넘지 않도록 초기화 (예: 상태창 길이 제한)
			if (current_info_y > MAP_HEIGHT) { // 상태창 출력 높이 제한 예시
				current_info_y = 2;
			}
		
	}
}