
// 1 ~ 2번 완료
// 3번 - 샌드웜이 가장 가까운 유닛을 찾아 이동. 하지만 이후 더이상 가까운 유닛을 찾아 이동하지 않음 (수정 필요)

#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <limits.h>
#include "common.h"
#include "io.h"
#include "display.h"

void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir);
void sample_obj_move(OBJECT_SAMPLE* obj);
void place_object(int, int, int, OBJECT_SAMPLE);
POSITION sample_obj_next_position(OBJECT_SAMPLE* obj);
POSITION find_nearest_unit(POSITION obj_pos, char map[1][MAP_HEIGHT][MAP_WIDTH]);


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)  //일정한 간격 (TICK = 10ms)마다 증가
CURSOR cursor = { { 1, 1 }, {1, 1} };   //직전위치, 현재위치


/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

RESOURCE resource = {
	.spice = 5,   //현재 보유한 스파이스
	.spice_max = 9,  //스파이스 최대 저장량
	.population = 0,   //현재 인구 수
	.population_max = 0   //최대 인구 수
};

OBJECT_SAMPLE h_base = {
	.pos = {1, MAP_WIDTH - 3},
	.dest = {0, 0},
	.repr = 'B',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE h_plate = {
	.pos = {1, MAP_WIDTH - 5},
	.dest = {0, 0},
	.repr = 'P',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE h_spice = {
	.pos = {MAP_HEIGHT - 11, MAP_WIDTH - 2},
	.dest = {0, 0},
	.repr = 'S',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE h_harvester = {
	.pos = {3, MAP_WIDTH - 2},
	.dest = {0, 0},
	.repr = 'H',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 2000,
	.cost = 5,
	.population = 5,
	.attack = 0,
	.attack_period = 0,
	.health = 70,
	.vision = 0,
	.command = 'H',
};

OBJECT_SAMPLE rock1 = {
	.pos = {3, MAP_WIDTH - 34},
	.dest = {0, 0},
	.repr = 'R',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE rock2 = {
	.pos = {4, MAP_WIDTH - 12},
	.dest = {0, 0},
	.repr = 'R',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE rock3 = {
	.pos = {9, MAP_WIDTH - 44},
	.dest = {0, 0},
	.repr = 'R',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE rock4 = {
	.pos = {11, MAP_WIDTH - 30},
	.dest = {0, 0},
	.repr = 'R',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE rock5 = {
	.pos = {12, MAP_WIDTH - 9},
	.dest = {0, 0},
	.repr = 'R',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE p_base = {
	.pos = {MAP_HEIGHT - 3, 1},
	.dest = {0, 0},
	.repr = 'B',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE p_plate = {
	.pos = {MAP_HEIGHT - 3, 3},
	.dest = {0, 0},
	.repr = 'P',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE p_spice = {
	.pos = {MAP_HEIGHT - 9, 1},
	.dest = {0, 0},
	.repr = 'S',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE p_harvester = {
	.pos = {MAP_HEIGHT - 4, 1},
	.dest = {0, 0},
	.repr = 'H',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE p_harvester2 = {
	.pos = {MAP_HEIGHT - 13, 20},
	.dest = {0, 0},
	.repr = 'H',  //화면에 표시될 문자
	.speed = 0,
	.next_move_time = 0
};

OBJECT_SAMPLE sandwarm1 = {
	.pos = {2, MAP_WIDTH - 48},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 3},
	.repr = 'W',  //화면에 표시될 문자
	.speed = 300,
	.next_move_time = 300
};


OBJECT_SAMPLE sandwarm2 = {
	.pos = {10, MAP_WIDTH - 16},
	.dest = {0, 0},
	.repr = 'W',  //화면에 표시될 문자
	.speed = 300,
	.next_move_time = 300
};


/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));
	int is_update_requested = 0;
	int reset = 0;

	init();  //맵과 지형 생성
	intro();  // "DUNE 1.5"
	display(resource, map, cursor, is_update_requested, reset);

	while (1) {
		// loop 돌 때마다(즉, TICK==10ms마다) 키 입력 확인
		KEY key = get_key();

		// 키 입력이 있으면 처리
		if (is_arrow_key(key)) {   //방향키인지 확인하는 함수
			cursor_move(ktod(key));   //맞다면 커서 이동  Ktod: 키를 방향으로 변환
		}
		else {
			// 방향키 외의 입력
			switch (key) {
			case k_quit: outro(); break;
			case k_none: break;
			case k_undef: break;
			case k_space:is_update_requested = 1; break;
			case k_escape:reset = 1;  break;
			default: break;
			}
		}

		// 샘플 오브젝트 동작
		sample_obj_move(&sandwarm1);
		sample_obj_move(&sandwarm2);

		// 화면 출력
		display(resource, map, cursor, is_update_requested, reset);
		is_update_requested = 0;
		reset = 0;
		Sleep(TICK);
		sys_clock += 10;
	}
}

/* ================= subfunctions =================== */
void intro(void) {
	printf("DUNE 1.5\n");
	Sleep(2000);
	system("cls");
}

void outro(void) {
	printf("exiting...\n");
	exit(0);
}

void place_object(int layer, int width, int height, OBJECT_SAMPLE obj) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			map[layer][obj.pos.row + i][obj.pos.column + j] = obj.repr;
		}
	}
}

void init(void) {
	// layer 0(map[0])에 지형 생성
	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';   //위 벽
		map[0][MAP_HEIGHT - 1][j] = '#';   //아래 벽
	}

	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		map[0][i][0] = '#';  //왼쪽 세로벽
		map[0][i][MAP_WIDTH - 1] = '#';    //오른쪽 세로벽


		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = ' ';   //가운데 빈 공간
		}
	}

	//하코넨 본진
	place_object(0, 2, 2, h_base);

	//하코넨 장판
	place_object(0, 2, 2, h_plate);

	//하코넨 스파이스
	place_object(0, 1, 1, h_spice);

	//rock
	place_object(0, 2, 2, rock1);
	place_object(0, 1, 1, rock2);
	place_object(0, 1, 1, rock3);
	place_object(0, 2, 2, rock4);
	place_object(0, 1, 1, rock5);

	//플레이어 본진
	place_object(0, 2, 2, p_base);

	//플레이어 장판
	place_object(0, 2, 2, p_plate);

	//플레이어 스파이스
	place_object(0, 1, 1, p_spice);


	// layer 1(map[1])은 비워 두기(-1로 채움)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}
	//하코넨 하베스터
	place_object(1, 1, 1, h_harvester);

	//플레이어 하베스터
	place_object(1, 1, 1, p_harvester);
	place_object(1, 1, 1, p_harvester2);

	//샌드웜
	place_object(1, 1, 1, sandwarm1);
	place_object(1, 1, 1, sandwarm2);

	// object sample
	//map[1][obj.pos.row][obj.pos.column] = 'o';
}


// (가능하다면) 지정한 방향으로 커서 이동
void cursor_move(DIRECTION dir) {
	static KEY last_key = k_none;            // 마지막으로 입력된 키 저장
	static int last_key_press_time = 0;      // 마지막 키 입력 시간을 기록
	int current_time = sys_clock;            // 현재 시간

	POSITION new_pos;

	if ((current_time - last_key_press_time) <= 150) {
		new_pos = pmove(cursor.current, dir);
		new_pos = pmove(new_pos, dir);
		new_pos = pmove(new_pos, dir);
	}
	else {
		new_pos = pmove(cursor.current, dir);
	}

	// validation check
	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 && \
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {
		cursor.previous = cursor.current;
		cursor.current = new_pos;
	}

	// 마지막 키와 시간 갱신
	last_key_press_time = current_time;

}

/* ================= sample object movement =================== */

// 가장 가까운 유닛 위치를 찾는 함수
POSITION find_nearest_unit(POSITION obj_pos, char map[1][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION nearest_pos = { -1, -1 };
	int min_distance = INT_MAX;

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			char cell = map[1][i][j];  // 정확한 레이어와 위치 참조
			// layer별 빈 공간 조건을 구분하여 적용
			if (cell != -1 && cell != 'W' && !(i == obj_pos.row && j == obj_pos.column)) {
				int distance = abs(i - obj_pos.row) + abs(j - obj_pos.column);

				// 더 가까운 오브젝트를 찾으면 위치와 최단 거리를 갱신
				if (distance < min_distance) {
					min_distance = distance;
					nearest_pos.row = i;
					nearest_pos.column = j;
				}
			}
		}
	}

	return nearest_pos;
}

POSITION sample_obj_next_position(OBJECT_SAMPLE* obj) {
	// 가장 가까운 유닛의 위치를 찾아 목적지로 설정
	POSITION nearest_unit_pos = find_nearest_unit(obj->pos, map);

	if (nearest_unit_pos.row != -1 && nearest_unit_pos.column != -1) {
		obj->dest = nearest_unit_pos;  // 가장 가까운 유닛을 목적지로 설정
	}
	else {
		// 유닛이 없을 경우 제자리에 멈춤
		return obj->pos;
	}

	POSITION diff = psub(obj->dest, obj->pos);  // 현재 위치와 목적지 간의 차이
	DIRECTION preferred_dir;

	// 목적지와의 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		preferred_dir = (diff.row > 0) ? d_down : d_up;
	}
	else {
		preferred_dir = (diff.column > 0) ? d_right : d_left;
	}

	// 이동 가능한 방향 찾기 (우회 포함)
	for (int i = 0; i < 4; i++) { // 최대 4방향 확인
		POSITION next_pos = pmove(obj->pos, preferred_dir);

		// 경계 체크 및 장애물 확인
		if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 &&
			1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 &&
			map[0][next_pos.row][next_pos.column] != 'R') {  // Rock 회피 조건
			return next_pos;  // 이동할 위치 반환
		}

		// 장애물로 인해 이동 불가 시 방향 전환
		preferred_dir = (DIRECTION)((preferred_dir % 4) + 1); // 시계 방향으로 변경
	}

	// 이동할 방향이 없으면 제자리 유지
	return obj->pos;
}


void sample_obj_move(OBJECT_SAMPLE* obj) {
	while (sys_clock > obj->next_move_time) {  // 이동 시간이 되었을 때만 반복 실행

		// 현재 위치를 빈 공간으로 설정 (샌드웜이 움직이기 전에 비우기)
		map[1][obj->pos.row][obj->pos.column] = -1; // 현재 위치 비움

		// 다음 위치 계산 및 업데이트
		POSITION new_pos = sample_obj_next_position(obj);

		// 이동할 위치가 없거나 멈춰야 할 경우, 현재 위치에 repr 표시
		if (new_pos.row == obj->pos.row && new_pos.column == obj->pos.column) {
			if (map[1][obj->pos.row][obj->pos.column] == -1) {
				map[1][obj->pos.row][obj->pos.column] = obj->repr; // 현재 위치에 다시 표시
			}
			break; // 이동 종료
		}

		// 새로운 위치가 현재 위치와 다를 때만 이동
		if (new_pos.row != obj->pos.row || new_pos.column != obj->pos.column) {
			obj->pos = new_pos;  // 샌드웜 위치 갱신
			map[1][obj->pos.row][obj->pos.column] = obj->repr;  // 새로운 위치에 샌드웜 설정
			obj->next_move_time = sys_clock + obj->speed;  // 다음 이동 시간 설정
		}

		// 현재 목적지에 도달했는지 확인하고, 도달했다면 유닛을 잡아먹고 새로운 유닛을 찾아 목적지를 갱신
		if (obj->pos.row == obj->dest.row && obj->pos.column == obj->dest.column) {
			// 유닛을 잡아먹고, 그 자리를 비움	
			map[1][obj->dest.row][obj->dest.column] = -1;  // 유닛 사라지게 함
			map[1][obj->dest.row][obj->dest.column] = obj->repr;

			// 현재 목적지에 도달했으므로, 새로운 유닛을 찾음
			POSITION nearest_object_pos = find_nearest_unit(obj->pos, map);
			if (nearest_object_pos.row != -1 && nearest_object_pos.column != -1) {
				obj->dest = nearest_object_pos;  // 새로운 유닛을 찾아 목적지 갱신
			}
		}

		// 이동할 위치가 없을 경우 이동을 중단
		if (new_pos.row == obj->pos.row && new_pos.column == obj->pos.column) {
			break;
		}
	}
}