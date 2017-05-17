//****************************************************************************
// Model: history.qm
// File:  ./history.h
//
// This code has been generated by QM tool (see state-machine.com/qm).
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// This code is covered by the following commercial QP license:
// License #   : QPCPP-EVAL-170201
// Issued to   : Company/individual evaluating the QP/C++ framework
// Framework(s): qpcpp
// Support ends: 2017-02-28
// Product(s)  :
// This license is available only for evaluation purposes and
// the generated code is still licensed under the terms of GPL.
// Please submit request for extension of the evaluaion period at:
// https://state-machine.com/licensing/index.html#RequestForm
//****************************************************************************
//${.::history.h} ............................................................
#ifndef history_h
#define history_h

enum ToastOvenSignals {
    OPEN_SIG = QP::Q_USER_SIG,
    CLOSE_SIG,
    TOAST_SIG,
    BAKE_SIG,
    OFF_SIG,
    TERMINATE_SIG // terminate the application
};

extern QP::QMsm * const the_oven; // opaque pointer to the oven MSM

#endif // history_h
