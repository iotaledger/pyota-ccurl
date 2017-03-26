#include <Python.h>
#include <stdint.h>

#define HASH_LENGTH 243
#define STATE_LENGTH 3 * HASH_LENGTH

// Define the size of a trit as 64 bits, so that we can compute 64 hashes in parallel.
typedef int64_t trit_t;

typedef struct {
  PyObject_HEAD
  trit_t _state[STATE_LENGTH];
} Curl;

static PyObject*
Curl_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  Curl *self;

  self = (Curl*)type->tp_alloc(type, 0);
  if (self != NULL) {
    memset(self->_state, 0, STATE_LENGTH * sizeof(trit_t));
  }

  return (PyObject*)self;
}

static PyObject*
Curl_absorb(Curl *self, PyObject *args, PyObject *kwds)
{
  /* Not implemented yet. */
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject*
Curl_squeeze(Curl *self, PyObject *args, PyObject *kwds)
{
  /* Not implemented yet. */
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject*
Curl_reset(Curl *self)
{
  /* Not implemented yet. */
  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef Curl_methods[] = {
  {"absorb", (PyCFunction)Curl_absorb, METH_VARARGS|METH_KEYWORDS, "Absorb trits into the sponge."},
  {"reset", (PyCFunction)Curl_reset, METH_NOARGS, "Resets internal state."},
  {"squeeze", (PyCFunction)Curl_squeeze, METH_VARARGS|METH_KEYWORDS, "Squeeze trits from the sponge."},
  {NULL} /* Sentinel */
};

static PyTypeObject ccurl_CurlType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "ccurl.Curl",       /* tp_name */
  sizeof(Curl),       /* tp_basicsize */
  0,                  /* tp_itemsize */
  0,                  /* tp_dealloc */
  0,                  /* tp_print */
  0,                  /* tp_getattr */
  0,                  /* tp_setattr */
  0,                  /* tp_as_async */
  0,                  /* tp_repr */
  0,                  /* tp_as_number */
  0,                  /* tp_as_sequence */
  0,                  /* tp_as_mapping */
  0,                  /* tp_hash  */
  0,                  /* tp_call */
  0,                  /* tp_str */
  0,                  /* tp_getattro */
  0,                  /* tp_setattro */
  0,                  /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT, /* tp_flags */
  "Curl",             /* tp_doc */
  0,                  /* tp_traverse */
  0,                  /* tp_clear */
  0,                  /* tp_richcompare */
  0,                  /* tp_weaklistoffset */
  0,                  /* tp_iter */
  0,                  /* tp_iternext */
  Curl_methods,       /* tp_methods */
  0,                  /* tp_members */
  0,                  /* tp_getset */
  0,                  /* tp_base */
  0,                  /* tp_dict */
  0,                  /* tp_descr_get */
  0,                  /* tp_descr_set */
  0,                  /* tp_dictoffset */
  0,                  /* tp_init */
  0,                  /* tp_alloc */
  Curl_new,           /* tp_new */
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
  PyObject *m, *hash_length;

  ccurl_CurlType.tp_new = PyType_GenericNew;
  if (PyType_Ready(&ccurl_CurlType) < 0)
    return NULL;

  m = PyModule_Create(&ccurlmodule);
  if (m == NULL)
    return NULL;

  hash_length = Py_BuildValue("i", HASH_LENGTH);
  if (hash_length == NULL)
    return NULL;

  // Define module-level symbols, named for compatibility with
  // pycurl module in PyOTA library.
  Py_INCREF(&ccurl_CurlType);
  PyModule_AddObject(m, "Curl", (PyObject*)&ccurl_CurlType);

  Py_INCREF(hash_length);
  PyModule_AddObject(m, "HASH_LENGTH", hash_length);

  return m;
}

