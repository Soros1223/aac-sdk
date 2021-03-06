/*
 * Copyright 2017-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include "AACE/Engine/Alexa/SpeechSynthesizerEngineImpl.h"
#include "AACE/Engine/Core/EngineMacros.h"

namespace aace {
namespace engine {
namespace alexa {

// String to identify log entries originating from this file.
static const std::string TAG("aace.alexa.SpeechSynthesizerEngineImpl");

SpeechSynthesizerEngineImpl::SpeechSynthesizerEngineImpl( std::shared_ptr<aace::alexa::SpeechSynthesizer> speechSynthesizerPlatformInterface ) :
    AudioChannelEngineImpl( speechSynthesizerPlatformInterface, TAG ) {
}

bool SpeechSynthesizerEngineImpl::initialize(
    std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::DirectiveSequencerInterface> directiveSequencer,
    std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::MessageSenderInterface> messageSender,
    std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::FocusManagerInterface> focusManager,
    std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::ContextManagerInterface> contextManager,
    std::shared_ptr<alexaClientSDK::avsCommon::avs::attachment::AttachmentManagerInterface> attachmentManager,
    std::shared_ptr<alexaClientSDK::avsCommon::avs::DialogUXStateAggregator> dialogUXStateAggregator,
    std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::CapabilitiesDelegateInterface> capabilitiesDelegate,
    std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::SpeakerManagerInterface> speakerManager,
    std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::ExceptionEncounteredSenderInterface> exceptionSender ) {
    
    try
    {
        ThrowIfNot( initializeAudioChannel( speakerManager ), "initializeAudioChannelFailed" );
    
        m_speechSynthesizerCapabilityAgent = alexaClientSDK::capabilityAgents::speechSynthesizer::SpeechSynthesizer::create( shared_from_this(), messageSender, focusManager, contextManager, exceptionSender, dialogUXStateAggregator );
        ThrowIfNull( m_speechSynthesizerCapabilityAgent, "couldNotCreateCapabilityAgent" );
        
        // add dialog state observer
        m_speechSynthesizerCapabilityAgent->addObserver( dialogUXStateAggregator );

        // add capability agent to the directive sequencer
        ThrowIfNot( directiveSequencer->addDirectiveHandler( m_speechSynthesizerCapabilityAgent ), "addDirectiveHandlerFailed" );

        // register capability with delegate
        ThrowIfNot( capabilitiesDelegate->registerCapability( m_speechSynthesizerCapabilityAgent ), "registerCapabilityFailed");

        return true;
    }
    catch( std::exception& ex ) {
        AACE_ERROR(LX(TAG,"initialize").d("reason", ex.what()));
        return false;
    }
}

std::shared_ptr<SpeechSynthesizerEngineImpl> SpeechSynthesizerEngineImpl::create(
    std::shared_ptr<aace::alexa::SpeechSynthesizer> speechSynthesizerPlatformInterface,
    std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::DirectiveSequencerInterface> directiveSequencer,
    std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::MessageSenderInterface> messageSender,
    std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::FocusManagerInterface> focusManager,
    std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::ContextManagerInterface> contextManager,
    std::shared_ptr<alexaClientSDK::avsCommon::avs::attachment::AttachmentManagerInterface> attachmentManager,
    std::shared_ptr<alexaClientSDK::avsCommon::avs::DialogUXStateAggregator> dialogUXStateAggregator,
    std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::CapabilitiesDelegateInterface> capabilitiesDelegate,
    std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::SpeakerManagerInterface> speakerManager,
    std::shared_ptr<alexaClientSDK::avsCommon::sdkInterfaces::ExceptionEncounteredSenderInterface> exceptionSender ) {
    
    try
    {
        std::shared_ptr<SpeechSynthesizerEngineImpl> speechSynthesizerEngineImpl = std::shared_ptr<SpeechSynthesizerEngineImpl>( new SpeechSynthesizerEngineImpl( speechSynthesizerPlatformInterface ) );
        
        ThrowIfNot( speechSynthesizerEngineImpl->initialize( directiveSequencer, messageSender, focusManager, contextManager, attachmentManager, dialogUXStateAggregator, capabilitiesDelegate, speakerManager, exceptionSender ), "initializeSpeechSynthesizerEngineImplFailed" );

        return speechSynthesizerEngineImpl;
    }
    catch( std::exception& ex ) {
        AACE_ERROR(LX(TAG,"create").d("reason", ex.what()));
        return nullptr;
    }
}

void SpeechSynthesizerEngineImpl::doShutdown()
{
    AudioChannelEngineImpl::doShutdown();

    if( m_speechSynthesizerCapabilityAgent != nullptr ) {
        m_speechSynthesizerCapabilityAgent->shutdown();
    }
}


} // aace::engine::alexa
} // aace::engine
} // aace

