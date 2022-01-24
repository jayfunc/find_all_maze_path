#include <iostream>
#include <vector>
#include <algorithm>

#define W PointType::Wall
#define NULL_POINT Point{-1, -1, -1}

struct Point {
	int x;
	int y;
	int value;
	bool operator==(const Point &other) const {
		return this->x == other.x && this->y == other.y && this->value == other.value;
	}

	bool operator!=(const Point &other) const {
		return this->x != other.x || this->y != other.y || this->value != other.value;
	}
};

enum PointType {
	Road,
	Fork,
	Enter,
	Exit,
	Corner,
	Wall,
};

class Solution {
	private:
	int width, height;

	int map[4][4] = {
		{Enter, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, Exit}
	};

	// 踪迹
	std::vector<Point> trace;

	Point previous_point = NULL_POINT;
	Point front_point = NULL_POINT;

	Point get_point(PointType type) {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (PointType(map[i][j]) == type) {
					return Point{i, j, type};
				}
			}
		}
		return NULL_POINT;
	}

	Point get_point(int x, int y) {
		if (x < 0 || x >= width || y < 0 || y >= height) {
			return NULL_POINT;
		} else {
			return Point{x, y, PointType(map[y][x])};
		}
	}

	bool can_go_back() {
		return trace.size() > 1;
	}

	bool go_back() {
		if (trace.size() <= 1) {
			previous_point = NULL_POINT;
			return false;
		} else {
			front_point = trace.back();
			trace.erase(remove(trace.begin(), trace.end(), trace.back()), trace.end());
			if (trace.size() - 1 - 1 >= 0) {
				previous_point = trace[trace.size() - 1 - 1];
			}
			else {
				previous_point = NULL_POINT;
			}
			return true;
		}
	}

	bool go_towards() {
		std::vector<Point> next_points = get_next_points();
		if (next_points.size() == 0) {
			return false;
		}
		previous_point = trace.back();
		trace.push_back(next_points.front());
		front_point = NULL_POINT;
		return true;
	}

	void go_towards(Point point) {
		if (trace.size() > 0) {
			previous_point = trace.back();
		}
		trace.push_back(point);
		front_point = NULL_POINT;
	}

	Point get_previous_point() {
		if (trace.size() - 1 - 1 < 0) {
			return NULL_POINT;
		}
		return trace[trace.size() - 1 - 1];
	}

	Point get_current_point() {
		if (trace.size() == 0) {
			return NULL_POINT;
		}
		else {
			return trace.back();
		}
	}

	bool is_current_point_fork() {
		return get_next_points().size() > 1;
	}

	bool is_current_point_corner() {
		return get_next_points().size() == 0;
	}

	bool is_current_point_exit() {
		return PointType(trace.back().value) == PointType::Exit;
	}

	std::vector<Point> get_next_points() {
		std::vector<Point> next_points;
		int current_x = get_current_point().x;
		int current_y = get_current_point().y;
		Point left_point = get_point(current_x - 1, current_y);
		Point top_point = get_point(current_x, current_y - 1);
		Point right_point = get_point(current_x + 1, current_y);
		Point bottom_point = get_point(current_x, current_y + 1);
		if (left_point != NULL_POINT && left_point.value != PointType::Wall && !contains(trace, left_point)) {
			next_points.push_back(left_point);
		}
		if (top_point != NULL_POINT && top_point.value != PointType::Wall && !contains(trace, top_point)) {
			next_points.push_back(top_point);
		}
		if (right_point != NULL_POINT && right_point.value != PointType::Wall && !contains(trace, right_point)) {
			next_points.push_back(right_point);
		}
		if (bottom_point != NULL_POINT && bottom_point.value != PointType::Wall && !contains(trace, bottom_point)) {
			next_points.push_back(bottom_point);
		}
		return next_points;
	}

	bool contains(std::vector<Point> points, Point point) {
		for	(int i = 0; i < points.size(); i++) {
			if (points[i] == point) {
				return true;
			}
		}
		return false;
	}

	int index_of(std::vector<Point> points, Point point) {
		std::vector<Point>::iterator it = std::find(points.begin(), points.end(), point);
		return it - points.begin();
	}

	public:
	Solution() {
		height = sizeof(map)/sizeof(map[0]);
		width = sizeof(map[0])/sizeof(int);
		map[0][0] = PointType::Enter;
		map[height - 1][width - 1] = PointType::Exit;
	}
	
	std::vector<int> history;

	void search_shortest_way() {
		Point enter_point = get_point(PointType::Enter);
		trace.push_back(enter_point);
		bool go_towards = true;
		int total_length = 1;
		do {
			//std::cout << "当前在 x = " << get_current_point().x << "; y = " << get_current_point().y << std::endl;
			if (is_current_point_exit()) {
				//std::cout << "Exit" << std::endl;
				history.push_back(total_length);
				if (!can_go_back()) {
					return;
				}
				go_back();
				go_towards = false;
				total_length--;
			}
			else if (is_current_point_corner()) {
				//std::cout << "当前点：死路" << std::endl;
				if (!can_go_back()) {
					return;
				}
				go_back();
				go_towards = false;
				total_length--;
			}
			else if (is_current_point_fork()) {
				//std::cout << "当前点：岔路口" << std::endl;
				std::vector<Point> next_points = get_next_points();
				if (front_point == NULL_POINT) {
					//std::cout << "当前选择：第 " << 1 << " 个分岔路口" << std::endl;
					this->go_towards();
					go_towards = true;
					total_length++;
				} else {
					if (front_point == next_points.back()) {
						//std::cout << "分岔路口选完了，往回走..." << std::endl;
						if (!can_go_back()) {
							return;
						}
						go_back();
						go_towards = false;
						total_length--;
					} else {
						int index = index_of(next_points, front_point);
						//std::cout << "当前选择：第 " << index + 1 + 1 << " 个分岔路口" << std::endl;
						this->go_towards(next_points[index + 1]);
						go_towards = true;
						total_length++;
					}
				}
			}
			else {
				if (go_towards) {
					this->go_towards();
					total_length++;
				} else {
					if (!can_go_back()) {
						return;
					}
					this->go_back();
					total_length--;
				}
			}
		} while (true);
	}
};

int main() {
	Solution solution;
	solution.search_shortest_way();
	std::cout << "寻路方案：" << solution.history.size() << std::endl;
	for (int i = 0; i < solution.history.size(); i++) {
		//std::cout << solution.history[i] << std::endl;
	}
	system("pause");
	return 0;
}
