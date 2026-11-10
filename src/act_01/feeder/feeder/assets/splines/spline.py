import math

from feeder.assets.splines.segment import Segment

class Spline:

    def __init__(self, points):
        self.alpha = 0.5    #[0.0, 1.0] - 0.5 For centripetal
        self.tension = 0.0  #[0.0, 1.0] - [Curve, Straight]

        self.segments = []

        size = len(points)
        for i in range(size-3):
            self.segments.append(self.__calc_segment(points[i], points[i+1], points[i+2], points[i+3]))

        self.__est_dist = 0
        for i in range(1, size - 2):
            self.__est_dist += (points[i+1] - points[i]).length()

    def __calc_segment(self, p0, p1, p2, p3):
        t01 = pow(p0.distance_to(p1), self.alpha)
        t12 = pow(p1.distance_to(p2), self.alpha)
        t23 = pow(p2.distance_to(p3), self.alpha)

        m1 = (1.0 - self.tension) * (p2 - p1 + t12 * ((p1 - p0) / t01 - (p2 - p0) / (t01 + t12)))
        m2 = (1.0 - self.tension) * (p2 - p1 + t12 * ((p3 - p2) / t23 - (p3 - p1) / (t12 + t23)))

        a = 2.0 * (p1 - p2) + m1 + m2
        b = -3.0 * (p1 - p2) - m1 - m1 - m2
        c = m1
        d = p1
        return Segment(a, b, c, d)

  # t = 0 start point of spline, 1 = end point of spline
    def get_point(self, t):
        segment_len = 1 / len(self.segments)
        frac, whole = math.modf(t / segment_len)

        segment = self.segments[int(whole)]
        frac2 = frac * frac
        frac3 = frac2 * frac
        return  segment.a * frac3 + segment.b * frac2 + segment.c * frac + segment.d

    @property
    def est_dist(self):
        return self.__est_dist