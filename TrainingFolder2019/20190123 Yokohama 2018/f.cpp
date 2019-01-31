#include <bits/stdc++.h>

using namespace std;
using pii = pair<int, int>;
using lint = long long;
using llf = long double;

template<typename T>
struct point {
	T x, y;
	point(T x_ = 0, T y_ = 0): x(x_), y(y_) { }
	llf norm() const { return hypotl(x, y); }
	point<T> operator+(const point<T> &p) const {
		return point<T>(x + p.x, y + p.y);
	}
	point<T> operator-(const point<T> &p) const {
		return point<T>(x - p.x, y - p.y);
	}
	point<llf> operator* (const double &v) const {
		return point<llf>(x * v, y * v);
	}
	point<llf> change_size(llf sz) const {
		llf _norm = norm();
		return point<llf>(x / _norm * sz, y / _norm * sz);
	}
};

lint ccw(point<lint> a, point<lint> b, point<lint> c) {
	return a.x * b.y + b.x * c.y + c.x * a.y - b.x * a.y - c.x * b.y - a.x * c.y;
}

struct line { 
	lint a, b, c; // ax+by+c=0

	template<typename T>
	llf dist_to_point(point<T> p) {
		return abs(a * p.x + b * p.y + c) / hypot(a, b);
	}

};

line line_through_points(point<lint> p, point<lint> q) {
	line ret;
	if(p.x == q.x) {
		ret.a = 1;
		ret.b = 0;
		ret.c = -p.x;
	}else {
		ret.a = p.y - q.y;
		ret.b = -(p.x - q.x);
		ret.c = (p.y - q.y) * (-p.x) + p.y * (p.x - q.x);
	}
	return ret;
}

int n;
point<lint> points[5050*2];
lint area[5050][5050];
lint tot_area;

line segments[5050];

llf find_x (int a0, llf x0, int a1) {
	point<llf> p = point<llf>(points[a0].x, points[a0].y) + (points[a0+1] - points[a0]).change_size(x0);
	llf h0 = segments[a0].dist_to_point(points[a1]);
	llf h1 = segments[a1].dist_to_point(p);
	//printf("%d %Lf %d %Lf %Lf\n", a0, x0, a1, h0, h1);
	return (tot_area / 2.0 - area[a0][a1] + x0 * h0) / h1;
	// area[a0][a1] / 2 + x0 * h / 2 + x1 * h / 2 = tot_area / 4
}

using plfi = pair<llf, int>;
vector<plfi> events[5050];

llf ans_min = 1e18, ans_max = -1e18;

void upd(llf x) {
	ans_min = min(ans_min, x);
	ans_max = max(ans_max, x);
}

int main() {
	scanf("%d", &n);
	for(int i = 0; i < n; i++) {
		scanf("%lld%lld", &points[i].x, &points[i].y);
		points[i+n] = points[i];
	}

	for(int i = 0; i < n; i++) {
		segments[i] = line_through_points(points[i], points[i+1]);
		//printf("%d: %lld %lld %lld\n", i, segments[i].a, segments[i].b, segments[i].c);

		for(int j = i+2; j < i+n; j++) {
			area[i][j % n] = area[i][(j+n-1) % n] + abs(ccw(points[i], points[(j+n-1)%n], points[j]));
		}
		assert(area[i][(i+n-1)%n] == area[0][n-1]);
	}
	tot_area = area[0][n-1];

	for(int i = 0, j = 0; i < n; i++) {
		while(area[i][j % n] * 2 <= tot_area) j += 1;
		//, printf("%d %d %lld %lld\n",i,j%n,area[i][j%n],tot_area/2);
		j--;
		llf x1 = find_x(i, 0, j % n);
		events[i].emplace_back(0.0, j);
		events[j % n].emplace_back(x1, i);
		//printf("%d %d %Lf\n", i, j%n, x1);
	}

	for(int i = 0; i < n; i++) {
		//events[i].emplace_back(0, -1);
		events[i].emplace_back((points[(i+1)%n] - points[i]).norm(), int(1e9));
		sort(events[i].begin(), events[i].end());
		for(auto &it : events[i]) it.second %= n;

		/*printf("%d: ", i);
		for(auto it : events[i]) printf("(%Lf, %d) ", it.first, it.second);
		puts("");*/
	}

	for(int i = 0; i < n; i++) {
		for(int e = 0; e + 1 < events[i].size(); e++) {
			llf low = events[i][e].first, high = events[i][e+1].first;
			int j = events[i][e].second;

			if(i == j) continue;

			point<llf> curi(points[i].x, points[i].y);
			point<llf> diri = (points[i+1] - points[i]).change_size(1);
			point<llf> curj(points[j].x, points[j].y);
			point<llf> dirj = (points[j+1] - points[j]).change_size(1);

			auto sz = [&i, &j, &curi, &curj, &diri, &dirj](llf m) {
				point<llf> p = curi + diri * m;
				point<llf> q = curj + dirj * find_x(i, m, j);
				llf ret = (p - q).norm();
				//printf("%d %d %Lf %Lf\n", i, j, m, ret);
				upd(ret);
				return ret;
			};

			sz(low);
			sz(high);

			const int THRES = 50;
			for(int rep = 0; rep < THRES; rep++) {
				llf m1 = (2 * low + high) / 3;
				llf m2 = (low + 2 * high) / 3;
				if(sz(m1) < sz(m2)) {
					high = m2;
				}else {
					low = m1;
				}
			}
		}
	}

	printf("%.20Lf\n%.20Lf\n", ans_min, ans_max);

	return 0;
}

/*
0: (0.000000, 1) (3.000000, 2) (6.000000, 1000000000) 
1: (0.000000, 2) (5.220153, 0) (10.440307, 1000000000) 
2: (0.000000, 0) (5.220153, 1) (10.440307, 1000000000) 
0 1 1.000000 6.800000
0 1 2.000000 7.701461
0 1 0.666667 6.703188
0 1 1.333333 6.987396
0 1 0.444444 6.681961
0 1 0.888889 6.758396
0 1 0.296296 6.684829
0 1 0.592593 6.692527
0 1 0.197531 6.693695
0 1 0.395062 6.681476
0 1 0.329218 6.683084
0 1 0.460905 6.682452
0 1 0.417010 6.681510
0 1 0.504801 6.684583
0 1 0.387746 6.681529
0 1 0.446273 6.682007
0 1 0.368237 6.681825
0 1 0.407255 6.681459
0 1 0.394249 6.681480
0 1 0.420261 6.681539
0 1 0.385578 6.681551
0 1 0.402920 6.681455
0 1 0.397139 6.681467
0 1 0.408700 6.681463
0 1 0.404846 6.681455
0 1 0.412554 6.681479
0 1 0.402277 6.681455
0 1 0.407416 6.681459
0 1 0.400565 6.681457
0 1 0.403990 6.681455
0 1 0.402848 6.681455
0 1 0.405132 6.681456
0 1 0.402087 6.681455
0 1 0.403609 6.681455
0 1 0.403102 6.681455
0 1 0.404117 6.681455
0 1 0.402764 6.681455
0 1 0.403440 6.681455
0 1 0.403215 6.681455
0 1 0.403666 6.681455
0 1 0.403515 6.681455
0 1 0.403816 6.681455
0 1 0.403415 6.681455
0 1 0.403616 6.681455
0 1 0.403348 6.681455
0 1 0.403482 6.681455
0 1 0.403438 6.681455
0 1 0.403527 6.681455
0 1 0.403497 6.681455
0 1 0.403556 6.681455
0 1 0.403477 6.681455
0 1 0.403517 6.681455
0 1 0.403464 6.681455
0 1 0.403490 6.681455
0 1 0.403482 6.681455
0 1 0.403499 6.681455
0 1 0.403493 6.681455
0 1 0.403505 6.681455
0 1 0.403489 6.681455
0 1 0.403497 6.681455
0 1 0.403487 6.681455
0 1 0.403492 6.681455
0 1 0.403490 6.681455
0 1 0.403494 6.681455
0 1 0.403489 6.681455
0 1 0.403491 6.681455
0 1 0.403491 6.681455
0 1 0.403492 6.681455
0 1 0.403490 6.681455
0 1 0.403491 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403491 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403491 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 1 0.403490 6.681455
0 2 4.000000 7.701461
0 2 5.000000 6.800000
0 2 4.666667 6.987396
0 2 5.333333 6.703188
0 2 5.111111 6.758396
0 2 5.555556 6.681961
0 2 5.407407 6.692527
0 2 5.703704 6.684829
0 2 5.604938 6.681476
0 2 5.802469 6.693695
0 2 5.539095 6.682452
0 2 5.670782 6.683084
0 2 5.495199 6.684583
0 2 5.582990 6.681510
0 2 5.553727 6.682007
0 2 5.612254 6.681529
0 2 5.592745 6.681459
0 2 5.631763 6.681825
0 2 5.579739 6.681539
0 2 5.605751 6.681480
0 2 5.597080 6.681455
0 2 5.614422 6.681551
0 2 5.591300 6.681463
0 2 5.602861 6.681467
0 2 5.587446 6.681479
0 2 5.595154 6.681455
0 2 5.592584 6.681459
0 2 5.597723 6.681455
0 2 5.596010 6.681455
0 2 5.599435 6.681457
0 2 5.594868 6.681456
0 2 5.597152 6.681455
0 2 5.596391 6.681455
0 2 5.597913 6.681455
0 2 5.595883 6.681455
0 2 5.596898 6.681455
0 2 5.596560 6.681455
0 2 5.597236 6.681455
0 2 5.596334 6.681455
0 2 5.596785 6.681455
0 2 5.596184 6.681455
0 2 5.596485 6.681455
0 2 5.596384 6.681455
0 2 5.596585 6.681455
0 2 5.596518 6.681455
0 2 5.596652 6.681455
0 2 5.596473 6.681455
0 2 5.596562 6.681455
0 2 5.596444 6.681455
0 2 5.596503 6.681455
0 2 5.596483 6.681455
0 2 5.596523 6.681455
0 2 5.596510 6.681455
0 2 5.596536 6.681455
0 2 5.596501 6.681455
0 2 5.596518 6.681455
0 2 5.596495 6.681455
0 2 5.596507 6.681455
0 2 5.596503 6.681455
0 2 5.596511 6.681455
0 2 5.596508 6.681455
0 2 5.596513 6.681455
0 2 5.596506 6.681455
0 2 5.596510 6.681455
0 2 5.596509 6.681455
0 2 5.596511 6.681455
0 2 5.596508 6.681455
0 2 5.596509 6.681455
0 2 5.596509 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596509 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596509 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
0 2 5.596510 6.681455
1 2 1.740051 4.892284
1 2 3.480102 4.330929
1 2 2.900085 4.254121
1 2 4.060119 4.761694
1 2 2.513407 4.371019
1 2 3.286763 4.268053
1 2 3.028978 4.243033
1 2 3.544549 4.360661
1 2 2.857121 4.261103
1 2 3.200835 4.252452
1 2 3.086263 4.243022
1 2 3.315406 4.274918
1 2 3.009883 4.243720
1 2 3.162644 4.247885
1 2 2.958962 4.247191
1 2 3.060803 4.242645
1 2 3.026856 4.243093
1 2 3.094750 4.243284
1 2 3.004225 4.243989
1 2 3.049487 4.242674
1 2 3.034400 4.242900
1 2 3.064575 4.242662
1 2 3.054517 4.242646
1 2 3.074633 4.242773
1 2 3.047811 4.242689
1 2 3.061222 4.242646
1 2 3.056752 4.242641
1 2 3.065693 4.242669
1 2 3.053772 4.242649
1 2 3.059732 4.242642
1 2 3.057745 4.242641
1 2 3.061719 4.242648
1 2 3.056421 4.242642
1 2 3.059070 4.242641
1 2 3.058187 4.242641
1 2 3.059953 4.242643
1 2 3.057598 4.242641
1 2 3.058776 4.242641
1 2 3.057206 4.242641
1 2 3.057991 4.242641
1 2 3.057729 4.242641
1 2 3.058252 4.242641
1 2 3.057555 4.242641
1 2 3.057903 4.242641
1 2 3.057787 4.242641
1 2 3.058020 4.242641
1 2 3.057710 4.242641
1 2 3.057865 4.242641
1 2 3.057813 4.242641
1 2 3.057916 4.242641
1 2 3.057882 4.242641
1 2 3.057951 4.242641
1 2 3.057859 4.242641
1 2 3.057905 4.242641
1 2 3.057889 4.242641
1 2 3.057920 4.242641
1 2 3.057879 4.242641
1 2 3.057900 4.242641
1 2 3.057893 4.242641
1 2 3.057907 4.242641
1 2 3.057888 4.242641
1 2 3.057897 4.242641
1 2 3.057894 4.242641
1 2 3.057900 4.242641
1 2 3.057892 4.242641
1 2 3.057896 4.242641
1 2 3.057895 4.242641
1 2 3.057898 4.242641
1 2 3.057894 4.242641
1 2 3.057896 4.242641
1 2 3.057894 4.242641
1 2 3.057895 4.242641
1 2 3.057894 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 2 3.057895 4.242641
1 0 6.960204 7.120003
1 0 8.700255 8.405620
1 0 6.380187 6.841488
1 0 7.540221 7.490619
1 0 5.993509 6.725415
1 0 6.766865 7.015279
1 0 5.735724 6.687114
1 0 6.251295 6.795725
1 0 5.563867 6.681776
1 0 5.907581 6.708859
1 0 5.449296 6.688115
1 0 5.678438 6.683435
1 0 5.602057 6.681464
1 0 5.754819 6.688748
1 0 5.551137 6.682076
1 0 5.652978 6.682400
1 0 5.517190 6.683365
1 0 5.585084 6.681494
1 0 5.562453 6.681804
1 0 5.607715 6.681492
1 0 5.592628 6.681459
1 0 5.622803 6.681661
1 0 5.582569 6.681513
1 0 5.602686 6.681466
1 0 5.595981 6.681455
1 0 5.609392 6.681504
1 0 5.591510 6.681462
1 0 5.600451 6.681460
1 0 5.597471 6.681455
1 0 5.603431 6.681469
1 0 5.595484 6.681455
1 0 5.599458 6.681457
1 0 5.594159 6.681457
1 0 5.596808 6.681455
1 0 5.595925 6.681455
1 0 5.597691 6.681455
1 0 5.595337 6.681455
1 0 5.596514 6.681455
1 0 5.596122 6.681455
1 0 5.596906 6.681455
1 0 5.595860 6.681455
1 0 5.596383 6.681455
1 0 5.596209 6.681455
1 0 5.596558 6.681455
1 0 5.596441 6.681455
1 0 5.596674 6.681455
1 0 5.596364 6.681455
1 0 5.596519 6.681455
1 0 5.596467 6.681455
1 0 5.596571 6.681455
1 0 5.596433 6.681455
1 0 5.596502 6.681455
1 0 5.596479 6.681455
1 0 5.596525 6.681455
1 0 5.596509 6.681455
1 0 5.596540 6.681455
1 0 5.596499 6.681455
1 0 5.596520 6.681455
1 0 5.596513 6.681455
1 0 5.596526 6.681455
1 0 5.596508 6.681455
1 0 5.596517 6.681455
1 0 5.596505 6.681455
1 0 5.596511 6.681455
1 0 5.596509 6.681455
1 0 5.596513 6.681455
1 0 5.596508 6.681455
1 0 5.596511 6.681455
1 0 5.596510 6.681455
1 0 5.596511 6.681455
1 0 5.596509 6.681455
1 0 5.596510 6.681455
1 0 5.596509 6.681455
1 0 5.596509 6.681455
1 0 5.596509 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
1 0 5.596509 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
1 0 5.596510 6.681455
2 0 1.740051 8.405620
2 0 3.480102 7.120003
2 0 2.900085 7.490619
2 0 4.060119 6.841488
2 0 3.673441 7.015279
2 0 4.446797 6.725415
2 0 4.189012 6.795725
2 0 4.704583 6.687114
2 0 4.532726 6.708859
2 0 4.876439 6.681776
2 0 4.761868 6.683435
2 0 4.991011 6.688115
2 0 4.685487 6.688748
2 0 4.838249 6.681464
2 0 4.787328 6.682400
2 0 4.889170 6.682076
2 0 4.855223 6.681494
2 0 4.923117 6.683365
2 0 4.832591 6.681492
2 0 4.877854 6.681804
2 0 4.817504 6.681661
2 0 4.847679 6.681459
2 0 4.837620 6.681466
2 0 4.857737 6.681513
2 0 4.830915 6.681504
2 0 4.844326 6.681455
2 0 4.839856 6.681460
2 0 4.848796 6.681462
2 0 4.836875 6.681469
2 0 4.842836 6.681455
2 0 4.840849 6.681457
2 0 4.844823 6.681455
2 0 4.843498 6.681455
2 0 4.846147 6.681457
2 0 4.842615 6.681455
2 0 4.844381 6.681455
2 0 4.843792 6.681455
2 0 4.844970 6.681455
2 0 4.843400 6.681455
2 0 4.844185 6.681455
2 0 4.843923 6.681455
2 0 4.844447 6.681455
2 0 4.843749 6.681455
2 0 4.844098 6.681455
2 0 4.843633 6.681455
2 0 4.843865 6.681455
2 0 4.843788 6.681455
2 0 4.843943 6.681455
2 0 4.843736 6.681455
2 0 4.843839 6.681455
2 0 4.843805 6.681455
2 0 4.843874 6.681455
2 0 4.843782 6.681455
2 0 4.843828 6.681455
2 0 4.843767 6.681455
2 0 4.843797 6.681455
2 0 4.843787 6.681455
2 0 4.843807 6.681455
2 0 4.843780 6.681455
2 0 4.843794 6.681455
2 0 4.843789 6.681455
2 0 4.843798 6.681455
2 0 4.843795 6.681455
2 0 4.843801 6.681455
2 0 4.843793 6.681455
2 0 4.843797 6.681455
2 0 4.843796 6.681455
2 0 4.843799 6.681455
2 0 4.843795 6.681455
2 0 4.843797 6.681455
2 0 4.843796 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843798 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 0 4.843797 6.681455
2 1 6.960204 4.330929
2 1 8.700255 4.892284
2 1 6.380187 4.761694
2 1 7.540221 4.254121
2 1 7.153543 4.268053
2 1 7.926899 4.371019
2 1 6.895758 4.360661
2 1 7.411329 4.243033
2 1 7.239472 4.252452
2 1 7.583186 4.261103
2 1 7.124901 4.274918
2 1 7.354043 4.243022
2 1 7.277662 4.247885
2 1 7.430424 4.243720
2 1 7.379503 4.242645
2 1 7.481344 4.247191
2 1 7.345556 4.243284
2 1 7.413450 4.243093
2 1 7.390819 4.242674
2 1 7.436082 4.243989
2 1 7.375731 4.242662
2 1 7.405907 4.242900
2 1 7.365673 4.242773
2 1 7.385790 4.242646
2 1 7.379084 4.242646
2 1 7.392495 4.242689
2 1 7.374614 4.242669
2 1 7.383555 4.242641
2 1 7.380574 4.242642
2 1 7.386535 4.242649
2 1 7.378588 4.242648
2 1 7.382561 4.242641
2 1 7.381237 4.242641
2 1 7.383886 4.242642
2 1 7.380354 4.242643
2 1 7.382120 4.242641
2 1 7.381531 4.242641
2 1 7.382708 4.242641
2 1 7.382316 4.242641
2 1 7.383101 4.242641
2 1 7.382054 4.242641
2 1 7.382578 4.242641
2 1 7.382403 4.242641
2 1 7.382752 4.242641
2 1 7.382287 4.242641
2 1 7.382519 4.242641
2 1 7.382442 4.242641
2 1 7.382597 4.242641
2 1 7.382390 4.242641
2 1 7.382494 4.242641
2 1 7.382356 4.242641
2 1 7.382425 4.242641
2 1 7.382402 4.242641
2 1 7.382448 4.242641
2 1 7.382386 4.242641
2 1 7.382417 4.242641
2 1 7.382407 4.242641
2 1 7.382427 4.242641
2 1 7.382400 4.242641
2 1 7.382414 4.242641
2 1 7.382409 4.242641
2 1 7.382418 4.242641
2 1 7.382406 4.242641
2 1 7.382412 4.242641
2 1 7.382410 4.242641
2 1 7.382414 4.242641
2 1 7.382409 4.242641
2 1 7.382411 4.242641
2 1 7.382411 4.242641
2 1 7.382412 4.242641
2 1 7.382412 4.242641
2 1 7.382413 4.242641
2 1 7.382411 4.242641
2 1 7.382412 4.242641
2 1 7.382411 4.242641
2 1 7.382412 4.242641
2 1 7.382411 4.242641
2 1 7.382412 4.242641
2 1 7.382411 4.242641
2 1 7.382412 4.242641
2 1 7.382411 4.242641
2 1 7.382412 4.242641
2 1 7.382411 4.242641
2 1 7.382412 4.242641
2 1 7.382411 4.242641
2 1 7.382412 4.242641
2 1 7.382412 4.242641
2 1 7.382412 4.242641
2 1 7.382412 4.242641
2 1 7.382412 4.242641
2 1 7.382412 4.242641
2 1 7.382412 4.242641
2 1 7.382412 4.242641
2 1 7.382412 4.242641
2 1 7.382412 4.242641
2 1 7.382412 4.242641
2 1 7.382412 4.242641
2 1 7.382412 4.242641
2 1 7.382412 4.242641
2 1 7.382412 4.242641
4.2426406871
8.4056198132


*/