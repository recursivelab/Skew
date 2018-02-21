#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <cmath>

struct Point
{
    double c[3];

    double operator()(int index) const
    {
        return c[index];
    }

    double& operator()(int index)
    {
        return c[index];
    }
};

struct Transform
{
    double c[3][3];

    Transform()
    {
        for (int row = 0; row < 3; ++row) {
            for (int column = 0; column < 3; ++column) {
                c[row][column] = row == column;
            }
        }
    }

    double operator()(int row, int column) const
    {
        return c[row][column];
    }

    double& operator()(int row, int column)
    {
        return c[row][column];
    }
};

inline Point operator*(const Transform &t, const Point &v)
{
    Point result;

    for (int row = 0; row < 3; ++row) {
        result(row) = 0;

        for (int column = 0; column < 3; ++column) {
            result(row) += t(row, column)*v(column);
        }
    }

    return result;
}

inline Transform transform(const Point x[4], const Point y[4])
{
    double matrix[12][13];
    int computedInRow[12];
    bool computed[13];
    bool usedRows[12];
    bool usedColumns[13];
    double s[13];

    for (int i = 0; i < 12; ++i) {
        for (int j = 0; j < 13; ++j) {
            matrix[i][j] = 0;
        }
    }

    int equation = 0;

    for (int row = 0; row < 3; ++row) {
        for (int column = 0; column < 4; ++column) {
            matrix[equation][9 + column] = -y[column](row);

            for (int i = 0; i < 3; ++i) {
                matrix[equation][3*row + i] = x[column](i);
            }

            computedInRow[equation] = -1;
            computed[equation] = false;
            usedRows[equation] = false;
            usedColumns[equation] = false;
            ++equation;
        }
    }

    usedColumns[12] = false;
    computed[12] = false;

    for (int iter = 0; iter < 12; ++iter) {
        double pivot = -1.0;
        int maxRow = -1;
        int maxColumn = -1;

        for (int r = 0; r < 12; ++r) {
            double sum = 0;

            for (int c = 0; c < 13; ++c) {
                sum += s[c]*matrix[r][c];
            }
        }

        for (int row = 0; row < 12; ++row) {
            if (usedRows[row]) {
                continue;
            }

            for (int column = 0; column < 13; ++column) {
                if (usedColumns[column]) {
                    continue;
                }

                double t = fabs(matrix[row][column]);

                if (t > pivot) {
                    pivot = t;
                    maxRow = row;
                    maxColumn = column;
                }
            }
        }

        if (pivot == 0) {
            throw(0);
        }

        usedRows[maxRow] = true;
        usedColumns[maxColumn] = true;
        computedInRow[maxRow] = maxColumn;
        computed[maxColumn] = true;

        double f = 1.0/matrix[maxRow][maxColumn];

        for (int column = 0; column < 13; ++column) {
            matrix[maxRow][column] *= f;
        }

        matrix[maxRow][maxColumn] = 1.0;

        for (int row = 0; row < 12; ++row) {
            double f = matrix[row][maxColumn];

            if (row != maxRow) {
                for (int column = 0; column < 13; ++column) {
                    matrix[row][column] -= f*matrix[maxRow][column];
                }
            }
        }
    }

    int freeVariable = 0;
    double max = -1.0;

    for (int variable = 0; variable < 13; ++variable) {
        if (computed[variable] == false) {
            freeVariable = variable;
        }
    }

    for (int equation = 0; equation < 12; ++equation) {
        double t = fabs(matrix[equation][freeVariable]);

        if (t > max) {
            max = t;
        }
    }

    double solution[13];
    double f = fabs(max) < 1.0 ? -1.0 : -1.0/max;

    solution[freeVariable] = -f;

    for (int equation = 0; equation < 12; ++equation) {
        if (usedRows[equation]) {
            int variable = computedInRow[equation];

            solution[variable] = matrix[equation][freeVariable]*f;
        }
    }

    int variable = 0;
    Transform result;

    for (int row = 0; row < 3; ++row) {
        for (int column = 0; column < 3; ++column) {
           result(row, column) = solution[variable];
           ++variable;
        }
    }

    return result;
}

#endif // TRANSFORM_H
