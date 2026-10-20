#include <iostream>
#include <windows.h>
#include <chrono>
#include <thread>
#include <string>
#include <cmath>


class Game
{
private:
    struct Vect
    {
        float x;
        float y;
        float z;
    };
    struct Vect2
    {
        int x;
        int y;
    };
    char** data;
    const int row = 30;
    const int col = 120;
    int cX = row / 2;
    int cY = col / 2;
  
public:
    Game()
    {
        data = new char*[row];
        for (int i = 0; i < row; ++i) {
            data[i] = new char[col](); 
        }
        for (int i = 0; i < row; ++i)
        {
            for (int j = 0; j < col; ++j)
            {
                data[i][j] = ' ';
            }
        }
    }
    void print()
    {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

        static std::string buf;
        buf.clear();
        buf.reserve((col + 1) * row*2);

        for (int i = 0; i < row; ++i)
        {
            for (int j = 0; j < col; ++j)
            {
                buf += data[i][j];
            }
            buf += '\n';
        }
        std::cout.write(buf.data(), buf.size());
        std::cout.flush();
    }
    void squareDraw(int a, int b)
    {
        int d = -1;
        int shift = 10;
        int csX = cX;
        int csY = cY;
        
        auto next = std::chrono::steady_clock::now();
        while (true)
        {
            csY += d;
            if (csY - b / 2 < -shift) { csY = b / 2 - shift;           d = 1; }
            if (csY + b / 2 >= col + shift) { csY = col - 1 - b / 2 + shift; d = -1; }
            for (int i = 0; i < row; ++i)
            {
                for (int j = 0; j < col; ++j)
                {
                    if ((i >= csX - a / 2 && i <= csX + a / 2) &&
                        (j >= csY - b / 2 && j <= csY + b / 2))
                    {
                        data[i][j] = '@';
                    }
                    else
                    {
                        data[i][j] = ' ';
                    }
                }

            }
            next += std::chrono::milliseconds(20);
            std::this_thread::sleep_until(next);
            print();

        }
    }
    
    void cub(float x, float y, float z)
    {
        int d = -1;
        auto next = std::chrono::steady_clock::now();
        double c = 0;
        double h = x / 2;
        const char gradient[] = ".:^=+*#";
        const int s = 7;
        int prevIndex[6] = { 0, 0, 0, 0, 0, 0 };

        Vect normals[6] = {
            {  1, 0, 0}, {-1, 0, 0},
            { 0,  1, 0}, { 0,-1, 0},
            { 0, 0,  1}, { 0, 0,-1}
        };
        while (true)
        {
            c+=0.05f;
            
            for (int i = 0; i < row; ++i)
            {
                for (int j = 0; j < col; ++j)
                {
                    data[i][j] = ' ';
                }

            }
            cY += d;
            if (cY < 0) { cY = 0;           d = 1; }
            if (cY >= col ) { cY = col ; d = -1; }
            for (int i = 0; i < row; ++i)
            {
                for (int j = 0; j < col; ++j)
                {
                    if (i > cX)
                    {
                        data[i][j] = '_';
                    }
                }

            }
            for (int f = 0; f < 6; ++f)
            {
                Vect n = normals[f];
                n = rotateY(n.x, n.y, n.z, c);
                n = rotateX(n.x, n.y, n.z, c);
                n = rotateZ(n.x, n.y, n.z, c);

                int newIndex = (int)((n.z + 1.0) / 2.0 * (s - 1));
                if (newIndex < 0) newIndex = 0;
                if (newIndex >= s) newIndex = s - 1;

                if (abs(newIndex - prevIndex[f]) >= 2)
                    prevIndex[f] = newIndex;

                char ch = gradient[prevIndex[f]];

                for (double u = -h; u <= h; u += 0.4)
                {
                    for (double v = -h; v <= h; v += 0.4)
                    {
                        if (f == 0) draw(h, u, v, c, ch);
                        if (f == 1) draw(-h, u, v, c, ch);
                        if (f == 2) draw(u, h, v, c, ch);
                        if (f == 3) draw(u, -h, v, c, ch);
                        if (f == 4) draw(u, v, h, c, ch);
                        if (f == 5) draw(u, v, -h, c, ch);
                    }
                }
            }
           
            next += std::chrono::milliseconds(30);
            std::this_thread::sleep_until(next);
            print();
        }
    }
    void draw(double h, double u, double v, double c, char ch)
    {
        Vect p = { h, u, v };
        
        //p = rotateX(p.x, p.y, p.z, c);
        p = rotateZ(p.x, p.y, p.z, c);  
        p = rotateY(p.x, p.y, p.z, c);

        p = rotateX(p.x, p.y, p.z, c);

        int si = cX + p.x * 2;
        int sj = cY + p.y * 2;
        
        if (si >= 0 && si < row && sj >= 0 && sj < col)
        {
            data[si][sj] = ch;
            
        }
    }
    
    Vect rotateX(double x, double y, double z, double alpha)
    {
        Vect coord;
        coord.x = x;                             
        coord.y = y * cos(alpha) - z * sin(alpha);
        coord.z = y * sin(alpha) + z * cos(alpha);
        return coord;
    }
    Vect rotateY(double x, double y, double z, double alpha)
    {
        Vect coord;
        coord.x = x * cos(alpha) + z * sin(alpha);
        coord.y = y;
        coord.z = -x * sin(alpha) + z * cos(alpha);
        return coord;

    }
    Vect rotateZ(double x, double y, double z, double alpha)
    {
        Vect coord;
        coord.x = x * cos(alpha) - y * sin(alpha);
        coord.y = x * sin(alpha) + y * cos(alpha);
        coord.z = z;
        return coord;
    }
    void sphere(int r)
    {
        int d = -1;
        char gradient[] = " .:-=+*#";
        int s = 8;
        auto next = std::chrono::steady_clock::now();
        double c = 0;
        const double dTheta = 0.28;   
        const double dPhi = 0.28;
       
        while (true)
        {
            c += 0.05f;
            for (int i = 0; i < row; ++i)
            {
                for (int j = 0; j < col; ++j)
                {
                    data[i][j] = ' ';
                }

            }
            cY += d;
            if (cY < 0) { cY = 0;           d = 1; }
            if (cY > col) { cY = col; d = -1; }
            for (double theta = 0.0; theta <= 3.14; theta += dTheta)
            {
                for (double phi = 0.0; phi <= 2.0 * 3.14; phi += dPhi)
                {
                    double x = r * sin(theta) * cos(phi);
                    double y = r * sin(theta) * sin(phi);
                    double z = r * cos(theta) ;
               
                    Vect p = { x, y, z };
                    p = rotateY(p.x, p.y, p.z, c);
                    p = rotateX(p.x, p.y, p.z, c);
                    p = rotateZ(p.x, p.y, p.z, c);

                    int si = cX + (int)(p.x * 1.2);
                    int sj = cY  + (int)(p.y * 1.5);
                    
                    double nx = p.x / r;
                    double ny = p.y / r;
                    double nz = p.z / r;
                    
                    int index = (int)(nz * (s - 1));
                    if (index < 0) index = 0;
                    if (index >= s) index = s - 1;
                    char ch = gradient[index];
                    //std::cout << nz << std::endl;
                    if (si >= 0 && si < row && sj >= 0 && sj < col && nz > 0)
                        data[si][sj] = ch;
                }
            }


            
            next += std::chrono::milliseconds(20);
            std::this_thread::sleep_until(next);
            print();
        }
    }
    void circle(int r)
    {
        int d = -1;
        int shift = 10;
        int csX = cX;
        int csY = cY;
        char gradient[] = "@%#*+=-:.";
        int s = std::size(gradient) - 1;
        auto next = std::chrono::steady_clock::now();
        while (true)
        {
            csY += d;
            if (csY - r < -shift) { csY = r - shift;           d = 1; }
            if (csY + r >= col + shift) { csY = col - 1 - r + shift; d = -1; }
            int c = 0;
            for (int i = 0; i < row; ++i)
            {
                for (int j = 0; j < col; ++j)
                {
                    int dx = (i - csX) * 2;
                    int dy = (j - csY);
                    int dist = dx * dx + dy * dy;
                    int maxr = r * r + r;
                    if (dist <= maxr)
                    {
                        //int index = maxr * s / dist;
                        int index = dist * s / maxr;
                        if (index >= s) index = s - 1;
                        data[i][j] = gradient[index];
                    }
                    else
                    {
                        data[i][j] = ' ';
                    }
                }
            }
            next += std::chrono::milliseconds(20);
            std::this_thread::sleep_until(next);
            print();
        }
    }
    void world()
    {
        const int c = 20;
        auto next = std::chrono::steady_clock::now();
        double r = 0;
        Vect2 p[c];
        for (int i = 0; i < c; ++i)
        {
            p[i].x = rand() % row;
            p[i].y = rand()  % col;
            int si = int(p[i].x);
            int sj = int(p[i].y);

        }
        char gradient[] = "$@#*+=-:.$@#*+=-:.";
        int s = 9;
        while (true)
        {
            r+=5;
           
            for (int i = 0; i < row; ++i)
            {
                for (int j = 0; j < col; ++j)
                {
                    int bestDist = r;
                    int bestIdx = 0;

                    for (int k = 0; k < c; ++k)
                    {
                        int dx = (i - p[k].x)*2;
                        int dy = (j - p[k].y);
                        int dist = dx * dx + dy * dy;   

                        if (dist < bestDist)
                        {
                            bestDist = dist;
                            bestIdx = k;
                        }
                    }

                    data[i][j] = gradient[bestIdx]; 
                }
            }
            next += std::chrono::milliseconds(30);
            std::this_thread::sleep_until(next);
            print();
        }
    }
    
    ~Game()
    {
        for (int i = 0; i < row; ++i)
        {
            delete[] data[i];
        }
        delete[] data;
    }
};

int main()
{
    srand(time(0));
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(hOut, &ci);
    ci.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &ci);
    Game g;
    //g.squareDraw(20, 40);
    //g.circle(30);
    g.cub(10, 15, 10);
    //g.sphere(10);
    //g.world(); шум воронова 
}
