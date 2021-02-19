#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "wigner.h"


static PyObject* c_wigner_3j_l(PyObject* self, PyObject* args)
{
    double l2, l3, m2, m3, l1min, l1max;
    double* thrcof;
    int ier, i, n;

    PyObject* list;
    PyObject* result;

    if(!PyArg_ParseTuple(args, "dddd", &l2, &l3, &m2, &m3))
        return NULL;

    ier = wigner_3j_l(l2, l3, m2, m3, &l1min, &l1max, NULL, 0);

    switch(ier)
    {
    case 0:
        break;

    case 1:
        PyErr_SetString(PyExc_ValueError, "either `l2 < abs(m2)` or "
                        "`l3 < abs(m3)");
        return NULL;

    case 2:
        PyErr_SetString(PyExc_ValueError, "either `l2+abs(m2)` or "
                        "`l3+abs(m3)` non-integer");
        return NULL;

    case 3:
        PyErr_SetString(PyExc_ValueError, "`l1max-l1min` not an integer");
        return NULL;

    case 4:
        PyErr_SetString(PyExc_ValueError, "`l1max` less than `l1min`");
        return NULL;

    default:
        PyErr_SetString(PyExc_RuntimeError, "unknown error");
        return NULL;
    }

    n = l1max-l1min+1.1;
    thrcof = malloc(n*sizeof(double));
    if(!thrcof)
        return PyErr_NoMemory();

    wigner_3j_l(l2, l3, m2, m3, &l1min, &l1max, thrcof, n);

    list = PyList_New(n);
    for(i = 0; i < n; ++i)
        PyList_SetItem(list, i, PyFloat_FromDouble(thrcof[i]));

    free(thrcof);

    result = Py_BuildValue("ddO", l1min, l1max, list);

    Py_DECREF(list);

    return result;
}


static PyObject* c_wigner_3j_m(PyObject* self, PyObject* args)
{
    double l1, l2, l3, m1, m2min, m2max;
    double* thrcof;
    int ier, i, n;

    PyObject* list;
    PyObject* result;

    if(!PyArg_ParseTuple(args, "dddd", &l1, &l2, &l3, &m1))
        return NULL;

    ier = wigner_3j_m(l1, l2, l3, m1, &m2min, &m2max, NULL, 0);

    switch(ier)
    {
    case 0:
        break;

    case 1:
        PyErr_SetString(PyExc_ValueError, "either `l1 < abs(m1)` or "
                        "`l1+abs(m1)` non-integer");
        return NULL;

    case 2:
        PyErr_SetString(PyExc_ValueError, "`abs(l1-l2) <= l3 <= l1+l2` "
                        "not satisfied");
        return NULL;

    case 3:
        PyErr_SetString(PyExc_ValueError, "`l1+l2+l3` not an integer");
        return NULL;

    case 4:
        PyErr_SetString(PyExc_ValueError, "`m2max-m2min` not an integer");
        return NULL;

    case 5:
        PyErr_SetString(PyExc_ValueError, "`m2max` less than `m2min`");
        return NULL;

    default:
        PyErr_SetString(PyExc_RuntimeError, "unknown error");
        return NULL;
    }

    n = m2max-m2min+1.1;
    thrcof = malloc(n*sizeof(double));
    if(!thrcof)
        return PyErr_NoMemory();

    wigner_3j_m(l1, l2, l3, m1, &m2min, &m2max, thrcof, n);

    list = PyList_New(n);
    for(i = 0; i < n; ++i)
        PyList_SetItem(list, i, PyFloat_FromDouble(thrcof[i]));

    free(thrcof);

    result = Py_BuildValue("ddO", m2min, m2max, list);

    Py_DECREF(list);

    return result;
}


static PyObject* c_wigner_d_l(PyObject* self, PyObject* args)
{
    int lmin, lmax, m1, m2, i, n;
    double theta;
    double* d;
    PyObject* list;

    if(!PyArg_ParseTuple(args, "iiiid", &lmin, &lmax, &m1, &m2, &theta))
        return NULL;

    if(lmin < 0 || lmax < lmin)
    {
        PyErr_SetString(PyExc_ValueError, "requires 0 <= lmin <= lmax");
        return NULL;
    }

    n = lmax-lmin+1;
    d = malloc(n*sizeof(double));
    if(!d)
        return PyErr_NoMemory();

    wigner_d_l(lmin, lmax, m1, m2, theta, d);

    list = PyList_New(n);
    for(i = 0; i < n; ++i)
        PyList_SetItem(list, i, PyFloat_FromDouble(d[i]));

    free(d);

    return list;
}


static PyMethodDef methods[] = {
    {"wigner_3j_l", c_wigner_3j_l, METH_VARARGS, PyDoc_STR(
        "wigner_3j_l(l2, l3, m2, m3)\n"
        "--\n"
        "\n"
        "Returns\n"
        "-------\n"
        "l1min : float\n"
        "    Smallest allowable l1 in 3j symbol.\n"
        "l1max : float\n"
        "    Largest allowable l1 in 3j symbol.\n"
        "thrcof : list of float\n"
        "    Set of 3j coefficients generated by evaluating the 3j symbol\n"
        "    for all allowed values of l1.\n"
    )},
    {"wigner_3j_m", c_wigner_3j_m, METH_VARARGS, PyDoc_STR(
        "wigner_3j_m(l1, l2, l3, m1)\n"
        "--\n"
        "\n"
        "Returns\n"
        "-------\n"
        "m2min : float\n"
        "    Smallest allowable m2 in 3j symbol.\n"
        "m2max : float\n"
        "    Largest allowable m2 in 3j symbol.\n"
        "thrcof : list of float\n"
        "    Set of 3j coefficients generated by evaluating the 3j symbol\n"
        "    for all allowed values of m2.\n"
    )},
    {"wigner_d_l", c_wigner_d_l, METH_VARARGS, PyDoc_STR(
        "wigner_d_l(lmin, lmax, m1, m2, theta)\n"
        "--\n"
        "\n"
        "Returns\n"
        "-------\n"
        "list of float\n"
        "    Values `d^{l}_{m1, m2}` where `l = lmin, ..., lmax`.\n"
    )},
    {NULL, NULL}
};


static struct PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "c",
    PyDoc_STR("C library bindings"),
    -1,
    methods
};


PyMODINIT_FUNC
PyInit_c(void)
{
    return PyModule_Create(&module);
}
