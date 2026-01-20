#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/client/src/lib/pca.h"

array<array<double>> computePca(array<array<double>> data, double dims)
{
    if (data->get_length() == 0) return array<any>();
    shared dim = const_(data)[0]->get_length();
    shared mean = Array(dim)->fill(0);
    for (auto& vec : data)
    {
        for (auto i = 0; i < dim; i++)
        const_(mean)[i] += const_(vec)[i];
    }
    for (auto i = 0; i < dim; i++)
    const_(mean)[i] /= data->get_length();
    auto centered = data->map([=](auto v) mutable
    {
        return v->map([=](auto val, auto idx) mutable
        {
            return val - const_(mean)[idx];
        }
        );
    }
    );
    auto cov = Array->from(object{
        object::pair{std::string("length"), dim}
    }, [=]() mutable
    {
        return Array(dim)->fill(0);
    }
    );
    for (auto& vec : centered)
    {
        for (auto i = 0; i < dim; i++)
        {
            for (auto j = 0; j < dim; j++)
            {
                const_(const_(cov)[i])[j] += const_(vec)[i] * const_(vec)[j];
            }
        }
    }
    for (auto i = 0; i < dim; i++)
    {
        for (auto j = 0; j < dim; j++)
        {
            const_(const_(cov)[i])[j] /= data->get_length() - 1;
        }
    }
    shared eigenvectors = array<array<double>>();
    auto matrix = cov->map([=](auto row) mutable
    {
        return row->slice();
    }
    );
    for (auto k = 0; k < dims; k++)
    {
        auto vec = Array(dim)->fill(1 / Math->sqrt(dim));
        for (auto iter = 0; iter < 50; iter++)
        {
            auto next = multiplyMatrixVector(matrix, vec);
            shared norm = Math->sqrt(next->reduce([=](auto a, auto b) mutable
            {
                return a + b * b;
            }
            , 0));
            if (norm == 0) break;
            vec = next->map([=](auto v) mutable
            {
                return v / norm;
            }
            );
        }
        eigenvectors->push(vec->slice());
        auto lambda = dot(vec, multiplyMatrixVector(matrix, vec));
        for (auto i = 0; i < dim; i++)
        {
            for (auto j = 0; j < dim; j++)
            {
                const_(const_(matrix)[i])[j] -= lambda * const_(vec)[i] * const_(vec)[j];
            }
        }
    }
    return centered->map([=](auto vec) mutable
    {
        return eigenvectors->map([=](auto ev) mutable
        {
            return dot(vec, ev);
        }
        );
    }
    );
};


array<double> multiplyMatrixVector(array<array<double>> mat, array<double> vec)
{
    return mat->map([=](auto row) mutable
    {
        return row->reduce([=](auto sum, auto val, auto i) mutable
        {
            return sum + val * const_(vec)[i];
        }
        , 0);
    }
    );
};


double dot(array<double> a, array<double> b)
{
    return a->reduce([=](auto sum, auto val, auto i) mutable
    {
        return sum + val * const_(b)[i];
    }
    , 0);
};


