//****************************************************************************
// Model: game.qm
// File:  ./ship.cpp
//
// This code has been generated by QM tool (see state-machine.com/qm).
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
// for more details.
//****************************************************************************
//${.::ship.cpp} .............................................................
#include "qpcpp.h"
#include "bsp.h"
#include "game.h"

//Q_DEFINE_THIS_FILE

#define SHIP_WIDTH  5U
#define SHIP_HEIGHT 3U

// encapsulated delcaration of the Ship active object ------------------------
namespace GAME {


#if ((QP_VERSION < 580) || (QP_VERSION != ((QP_RELEASE^4294967295) % 0x3E8)))
#error qpcpp version 5.8.0 or higher required
#endif

//${AOs::Ship} ...............................................................
class Ship : public QP::QActive {
private:
    uint8_t m_x;
    uint8_t m_y;
    uint8_t m_exp_ctr;
    uint16_t m_score;

public:
    Ship();

protected:
    static QP::QState initial(Ship * const me, QP::QEvt const * const e);
    static QP::QState active(Ship * const me, QP::QEvt const * const e);
    static QP::QState parked(Ship * const me, QP::QEvt const * const e);
    static QP::QState flying(Ship * const me, QP::QEvt const * const e);
    static QP::QState exploding(Ship * const me, QP::QEvt const * const e);
};

} // namespace GAME

namespace GAME {

// local objects -------------------------------------------------------------
static Ship l_ship; // the sole instance of the Ship active object

// Public-scope objects ------------------------------------------------------
QP::QActive * const AO_Ship = &l_ship; // opaque pointer
} // namespace GAME

// Active object definition --------------------------------------------------
namespace GAME {

//${AOs::Ship} ...............................................................
//${AOs::Ship::Ship} .........................................................
Ship::Ship()
 : QActive(Q_STATE_CAST(&Ship::initial)),
   m_x(GAME_SHIP_X),
   m_y(GAME_SHIP_Y)
{}

//${AOs::Ship::SM} ...........................................................
QP::QState Ship::initial(Ship * const me, QP::QEvt const * const e) {
    // ${AOs::Ship::SM::initial}
    me->subscribe(TIME_TICK_SIG);
    me->subscribe(PLAYER_TRIGGER_SIG);
    // object dictionaries...
    QS_OBJ_DICTIONARY(&l_ship);
    // function dictionaries for Ship HSM...
    QS_FUN_DICTIONARY(&Ship::initial);
    QS_FUN_DICTIONARY(&Ship::active);
    QS_FUN_DICTIONARY(&Ship::parked);
    QS_FUN_DICTIONARY(&Ship::flying);
    QS_FUN_DICTIONARY(&Ship::exploding);
    // local signals...
    QS_SIG_DICTIONARY(PLAYER_SHIP_MOVE_SIG, &l_ship);
    QS_SIG_DICTIONARY(TAKE_OFF_SIG,         &l_ship);
    QS_SIG_DICTIONARY(HIT_WALL_SIG,         &l_ship);
    QS_SIG_DICTIONARY(HIT_MINE_SIG,         &l_ship);
    QS_SIG_DICTIONARY(DESTROYED_MINE_SIG,   &l_ship);

    (void)e; // unused parameter
    return Q_TRAN(&active);
}
//${AOs::Ship::SM::active} ...................................................
QP::QState Ship::active(Ship * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        // ${AOs::Ship::SM::active::initial}
        case Q_INIT_SIG: {
            status_ = Q_TRAN(&parked);
            break;
        }
        // ${AOs::Ship::SM::active::PLAYER_SHIP_MOVE}
        case PLAYER_SHIP_MOVE_SIG: {
            me->m_x = Q_EVT_CAST(ObjectPosEvt)->x;
            me->m_y = Q_EVT_CAST(ObjectPosEvt)->y;
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&top);
            break;
        }
    }
    return status_;
}
//${AOs::Ship::SM::active::parked} ...........................................
QP::QState Ship::parked(Ship * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        // ${AOs::Ship::SM::active::parked::TAKE_OFF}
        case TAKE_OFF_SIG: {
            status_ = Q_TRAN(&flying);
            break;
        }
        default: {
            status_ = Q_SUPER(&active);
            break;
        }
    }
    return status_;
}
//${AOs::Ship::SM::active::flying} ...........................................
QP::QState Ship::flying(Ship * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        // ${AOs::Ship::SM::active::flying}
        case Q_ENTRY_SIG: {
            me->m_score = 0U; /* reset the score */
            AO_Tunnel->POST(Q_NEW(ScoreEvt, SCORE_SIG, me->m_score), me);
            status_ = Q_HANDLED();
            break;
        }
        // ${AOs::Ship::SM::active::flying::TIME_TICK}
        case TIME_TICK_SIG: {
            // tell the Tunnel to draw the Ship and test for hits
            AO_Tunnel->POST(Q_NEW(ObjectImageEvt, SHIP_IMG_SIG,
                                  me->m_x, me->m_y, SHIP_BMP),
                            me);
            ++me->m_score; // increment the score for surviving another tick

            if ((me->m_score % 10U) == 0U) { // is the score "round"?
                AO_Tunnel->POST(Q_NEW(ScoreEvt, SCORE_SIG, me->m_score), me);
            }
            status_ = Q_HANDLED();
            break;
        }
        // ${AOs::Ship::SM::active::flying::PLAYER_TRIGGER}
        case PLAYER_TRIGGER_SIG: {
            AO_Missile->POST(Q_NEW(ObjectPosEvt, MISSILE_FIRE_SIG,
                                   me->m_x, me->m_y + SHIP_HEIGHT - 1U),
                             me);
            status_ = Q_HANDLED();
            break;
        }
        // ${AOs::Ship::SM::active::flying::DESTROYED_MINE}
        case DESTROYED_MINE_SIG: {
            me->m_score += Q_EVT_CAST(ScoreEvt)->score;
            // the score will be sent to the Tunnel by the next TIME_TICK
            status_ = Q_HANDLED();
            break;
        }
        // ${AOs::Ship::SM::active::flying::HIT_WALL}
        case HIT_WALL_SIG: {
            status_ = Q_TRAN(&exploding);
            break;
        }
        // ${AOs::Ship::SM::active::flying::HIT_MINE}
        case HIT_MINE_SIG: {
            status_ = Q_TRAN(&exploding);
            break;
        }
        default: {
            status_ = Q_SUPER(&active);
            break;
        }
    }
    return status_;
}
//${AOs::Ship::SM::active::exploding} ........................................
QP::QState Ship::exploding(Ship * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        // ${AOs::Ship::SM::active::exploding}
        case Q_ENTRY_SIG: {
            me->m_exp_ctr = 0U;
            status_ = Q_HANDLED();
            break;
        }
        // ${AOs::Ship::SM::active::exploding::TIME_TICK}
        case TIME_TICK_SIG: {
            // ${AOs::Ship::SM::active::exploding::TIME_TICK::[me->m_exp_ctr<15U]}
            if (me->m_exp_ctr < 15U) {
                ++me->m_exp_ctr;
                // tell the Tunnel to draw the current stage of Explosion
                AO_Tunnel->POST(Q_NEW(ObjectImageEvt, EXPLOSION_SIG,
                                      me->m_x, (int8_t)((int)me->m_y - 4U + SHIP_HEIGHT),
                                      EXPLOSION0_BMP + (me->m_exp_ctr >> 2)),
                                me);
                status_ = Q_HANDLED();
            }
            // ${AOs::Ship::SM::active::exploding::TIME_TICK::[else]}
            else {
                AO_Tunnel->POST(Q_NEW(ScoreEvt, GAME_OVER_SIG, me->m_score), me);
                status_ = Q_TRAN(&parked);
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&active);
            break;
        }
    }
    return status_;
}

} // namespace GAME