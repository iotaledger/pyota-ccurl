#include <Python.h>

typedef struct {
  PyObject_HEAD
} ccurl_CurlObject;

static PyTypeObject ccurl_CurlType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "ccurl.Curl",               /* tp_name */
  sizeof(ccurl_CurlObject),   /* tp_basicsize */
  0,                          /* tp_itemsize */
  0,                          /* tp_dealloc */
  0,                          /* tp_print */
  0,                          /* tp_getattr */
  0,                          /* tp_setattr */
  0,                          /* tp_as_async */
  0,                          /* tp_repr */
  0,                          /* tp_as_number */
  0,                          /* tp_as_sequence */
  0,                          /* tp_as_mapping */
  0,                          /* tp_hash  */
  0,                          /* tp_call */
  0,                          /* tp_str */
  0,                          /* tp_getattro */
  0,                          /* tp_setattro */
  0,                          /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT,         /* tp_flags */
  "Curl",                     /* tp_doc */
};

static PyModuleDef ccurlmodule = {
  PyModuleDef_HEAD_INIT,
  "ccurl",
  "C implementation of IOTA curl functionality.",
  -1,
  NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC
PyInit_ccurl(void)
{
  PyObject* m;

  ccurl_CurlType.tp_new = PyType_GenericNew;
  if (PyType_Ready(&ccurl_CurlType) < 0)
    return NULL;

  m = PyModule_Create(&ccurlmodule);
  if (m == NULL)
    return NULL;

  Py_INCREF(&ccurl_CurlType);
  PyModule_AddObject(m, "Curl", (PyObject *)&ccurl_CurlType);
  return m;
}

