// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#pragma once

#include "HermesData.hpp"

namespace Hermes
{
    namespace Spec
    {
        enum class EOriginator { eUPSTREAM, eDOWNSTREAM };
        constexpr std::size_t size(EOriginator) { return 2; }
        template<class S> S& operator<<(S& s, EOriginator e)
        {
            switch (e)
            {
            case eUPSTREAM: s << "eUPSTREAM"; return s;
            case eDOWNSTREAM: s << "eDOWNSTREAM"; return s;
            default: s << "UNKNOWN_ORIGINATOR:" << static_cast<int>(e); return s;
            }
        }

        enum class EEvent
        {
            eRESET,
            eDISCONNECTED,
            eCONNECTED,
            eBOARD_AVAILABE,
            eREVOKE_BOARD_AVAILABLE,
            eMACHINE_READY,
            eREVOKE_MACHINE_READY,
            eSTART_TRANSPORT,
            eSTOP_TRANSPORT,
            eTRANSPORT_FINISHED,
        };

        struct OriginatingEvent
        {
            OriginatingEvent(EOriginator originator, EEvent event) : m_originator(originator), m_event(event) {}
            EOriginator m_originator;
            EEvent m_event;

            template<class S>
            friend S& operator<<(S& s, const OriginatingEvent& oe)
            {
                s << '{' << oe.m_originator << ':' << oe.m_event << '}';
            }
        };

        static const OriginatingEvent cRESET_DOWNSTREAM{EOriginator::eUPSTREAM, EEvent::eRESET};
        static const OriginatingEvent cDISCONNECTED_FROM_DOWNSTREAM{EOriginator::eDOWNSTREAM, EEvent::eDISCONNECTED};
        static const OriginatingEvent cBOARD_AVAILABLE{EOriginator::eUPSTREAM, EEvent::eBOARD_AVAILABE};
        static const OriginatingEvent cREVOKE_BOARD_AVAILABLE{EOriginator::eUPSTREAM, EEvent::eREVOKE_BOARD_AVAILABLE};


        static const OriginatingEvent cDISCONNECTED_FROM_DOWNSTREAM{EOriginator::eDOWNSTREAM, EEvent::eDISCONNECTED};
        static const OriginatingEvent cRESET_DOWNSTREAM{EOriginator::eUPSTREAM, EEvent::eRESET};
        static const OriginatingEvent cDISCONNECTED_FROM_DOWNSTREAM{EOriginator::eDOWNSTREAM, EEvent::eDISCONNECTED};
        static const OriginatingEvent cRESET_DOWNSTREAM{EOriginator::eUPSTREAM, EEvent::eRESET};
        static const OriginatingEvent cDISCONNECTED_FROM_DOWNSTREAM{EOriginator::eDOWNSTREAM, EEvent::eDISCONNECTED};
        static const OriginatingEvent cRESET_DOWNSTREAM{EOriginator::eUPSTREAM, EEvent::eRESET};
        static const OriginatingEvent cDISCONNECTED_FROM_DOWNSTREAM{EOriginator::eDOWNSTREAM, EEvent::eDISCONNECTED};
        static const OriginatingEvent cRESET_DOWNSTREAM{EOriginator::eUPSTREAM, EEvent::eRESET};
        static const OriginatingEvent cDISCONNECTED_FROM_DOWNSTREAM{EOriginator::eDOWNSTREAM, EEvent::eDISCONNECTED};

        constexpr std::size_t size(EEvent) { return 9; }
        template<class S> S& operator<<(S& s, EEvent event)
        {
            switch (event)
            {
            case eRESET: s << "eRESET"; return s;
            case eCONNECTED: s << "eRESET"; return s;
            case eBOARD_AVAILABE: s << "eRESET"; return s;
            case eREVOKE_BOARD_AVAILABLE: s << "eRESET"; return s;
            case eMACHINE_READY: s << "eRESET"; return s;
            case eREVOKE_MACHINE_READY: s << "eRESET"; return s;
            case eSTART_TRANSPORT: s << "eRESET"; return s;
            case eSTOP_TRANSPORT: s << "eRESET"; return s;
            case eTRANSPORT_FINISHED: s << "eRESET"; return s;
            default: s << "UNKNOWN_EVENT:" << static_cast<int>(event); return s;
            }
        }

        // any event from upstream not in this list will transit to the initial state (eNOT_CONNECTED)
        // any event from downstream not in this list must be rejected by the state machine
        
        struct Transition
        {
            EState m_from;
            EEvent m_event;
            EState m_to;
            // std::function<void()> m_trigger;
        };
        using Transitions = std::vector<Transitions>;
        struct UpstreamSpec
        {
            static const EState cSTART_STATE = EState::eNOT_CONNECTED;

            static std::vector<EEvent> GetSignals()
            {
                return{EEvent::eMACHINE_READY, EEvent::eREVOKE_MACHINE_READY, EEvent::eSTART_TRANSPORT, EEvent::eSTOP_TRANSPORT};
            }

            static std::vector<EEvent> GetSlots()
            {
                return{EEvent::eBOARD_AVAILABE, EEvent::eREVOKE_BOARD_AVAILABLE, EEvent::eTRANSPORT_FINISHED};
            }

            Transitions GetTransitions()
            {
                // the reset signal will always transition back to the starting state
                // for any other signals not mentioned here, the transition must be rejected
                //
                // the eDISCONNECTED event will always transition back to the starting state
                // any slot not mentioned here will lead to a reset 
                return
                {
                    Transition{EState::eNOT_AVAILABLE_NOT_READY, EEvent::eCONNECTED, EState::eNOT_AVAILABLE_NOT_READY},

                    Transition{EState::eNOT_AVAILABLE_NOT_READY, EEvent::eBOARD_AVAILABE, EState::eBOARD_AVAILABLE},
                    Transition{EState::eMACHINE_READY, EEvent::eBOARD_AVAILABE, EState::eAVAILABLE_AND_READY},
                    Transition{EState::eBOARD_AVAILABLE, EEvent::eREVOKE_BOARD_AVAILABLE, EState::eNOT_AVAILABLE_NOT_READY},
                    Transition{EState::eAVAILABLE_AND_READY, EEvent::eREVOKE_BOARD_AVAILABLE, EState::eMACHINE_READY},

                    Transition{EState::eAVAILABLE_AND_READY, EEvent::eSTART_TRANSPORT, EState::eTRANSPORTING},
                    Transition{EState::eTRANSPORTING, EEvent::eSTOP_TRANSPORT, EState::eTRANSPORT_STOPPED},
                    Transition{EState::eTRANSPORTING, EEvent::eTRANSPORT_FINISHED, EState::eTRANSPORT_FINISHED},
                    Transition{EState::eTRANSPORT_FINISHED, EEvent::eSTOP_TRANSPORT, EState::eNOT_AVAILABLE_NOT_READY},
                    Transition{EState::eTRANSPORT_STOPPED, EEvent::eTRANSPORT_FINISHED, EState::eNOT_AVAILABLE_NOT_READY},

                    Transition{EState::eTRANSPORTING, EEvent::eBOARD_AVAILABE, EState::eTRANSPORTING},
                    Transition{EState::eTRANSPORTING, EEvent::eREVOKE_BOARD_AVAILABLE, EState::eTRANSPORTING},
                    Transition{EState::eTRANSPORT_STOPPED, EEvent::eBOARD_AVAILABE, EState::eTRANSPORT_STOPPED},
                    Transition{EState::eTRANSPORT_STOPPED, EEvent::eREVOKE_BOARD_AVAILABLE, EState::eTRANSPORT_STOPPED},
                };
            }



        };
    }
}
