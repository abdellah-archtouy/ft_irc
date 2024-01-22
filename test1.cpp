#include <iostream>


class Point
{
        int i;
    public:
        Point(){}
        Point(int tmp);
        friend Point operator+(Point, Point);
        friend Point operator%(Point, Point);
        Point operator%(Point &);
        Point &operator=(Point &);
        friend std::ostream &operator<<(std::ostream &out, Point b){
            out << b.i;
            return out;
        }
};


Point::Point(int tmp)
{
    i = tmp;
}

Point &Point::operator=(Point &t)
{
    i = t.i;
    return *this;
}

Point operator+(Point a, Point b){
    Point t(a.i + b.i);
    return t;
}
Point operator%(Point a, Point b){
    Point t(a.i % b.i);
    return t;
}

Point Point::operator%(Point &a){
    Point t(i % a.i);
    return t;
}

int main()
{
    Point a(5);
    Point b(4);

    Point t = a + b;
    Point t1 = operator+(a,operator+(b, t));
    Point t2 = a.operator%(b);
    Point t3 = operator%(a,b);
    std::cout << t << std::endl;
    std::cout << t1<< std::endl;
    std::cout << t2 << std::endl;
    std::cout << t3<< std::endl;
}

class A
{
    private:
    public:
        A(Point a, point b);
};

class B : public A
{
    private:
    public:
        B(){
            A(Point a, point b);
        }
        B(): A(Point a, point b){
        
        }

};