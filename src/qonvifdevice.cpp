#include "qonvifdevice.hpp"
#include "devicemanagement.h"
#include "mediamanagement.h"
#include "ptzmanagement.h"

///////////////////////////////////////////////////////////////////////////////
namespace device {
///////////////////////////////////////////////////////////////////////////////
class QOnvifDevicePrivate
{
public:
    QOnvifDevicePrivate(
        const QString _serviceAddress,
        const QString _username,
        const QString _password)
        : iuserName(_username), ipassword(_password) {
        ideviceManagement =
            new ONVIF::DeviceManagement{_serviceAddress, iuserName, ipassword};

        imediaManagement =
            new ONVIF::MediaManagement{_serviceAddress, iuserName, ipassword};

        iptzManagement =
            new ONVIF::PtzManagement{_serviceAddress, iuserName, ipassword};
    }
    ~QOnvifDevicePrivate() {
        delete ideviceManagement;
        delete imediaManagement;
        delete iptzManagement;
    }

    QString iuserName;
    QString ipassword;
    Data    idata;

    // onvif managers
    ONVIF::DeviceManagement* ideviceManagement;
    ONVIF::MediaManagement*  imediaManagement;
    ONVIF::PtzManagement*    iptzManagement;

    Data::ProbeData deviceProbeData() {
        return idata.probeData;
    }

    void setDeviceProbeData(Data::ProbeData _probeData) {
        idata.probeData = _probeData;
    }

    bool deviceDateAndTime(Data::DateTime& _datetime) {
        QScopedPointer<ONVIF::SystemDateAndTime> systemDateAndTime(
            ideviceManagement->getSystemDateAndTime());
        if (!systemDateAndTime)
            return false;
        idata.dateTime.localTime = systemDateAndTime->localTime();
        idata.dateTime.utcTime   = systemDateAndTime->utcTime();
        _datetime                = idata.dateTime;
        return true;
    }

    bool setDeviceDateAndTime(QDateTime _dateAndTime) {
        ONVIF::SystemDateAndTime systemDateAndTime;
        systemDateAndTime.setlocalTime(_dateAndTime);
        ideviceManagement->setSystemDateAndTime(&systemDateAndTime);
        return true;
    }

    bool setScopes(QString _name, QString _location) {
        ONVIF::SystemScopes systemScopes;
        systemScopes.setScopes(_name, _location);

        ideviceManagement->setDeviceScopes(&systemScopes);
        return true;
    }
    bool setVideoConfig(Data::MediaConfig::Video::EncoderConfig _videoConfig) {
        ONVIF::VideoEncoderConfiguration videoConfiguration;

        videoConfiguration.setToken(_videoConfig.token);
        videoConfiguration.setName(_videoConfig.name);
        videoConfiguration.setUseCount(_videoConfig.useCount);
        videoConfiguration.setEncoding(_videoConfig.encoding);
        videoConfiguration.setWidth(_videoConfig.width);
        videoConfiguration.setHeight(_videoConfig.height);
        videoConfiguration.setQuality(_videoConfig.quality);
        videoConfiguration.setFrameRateLimit(_videoConfig.frameRateLimit);
        videoConfiguration.setEncodingInterval(_videoConfig.encodingInterval);
        videoConfiguration.setBitrateLimit(_videoConfig.bitrateLimit);
        videoConfiguration.setGovLength(_videoConfig.govLength);
        videoConfiguration.setH264Profile(_videoConfig.h264Profile);
        videoConfiguration.setType(_videoConfig.type);
        videoConfiguration.setIpv4Address(_videoConfig.ipv4Address);
        videoConfiguration.setPort(_videoConfig.port);
        videoConfiguration.setTtl(_videoConfig.ttl);
        videoConfiguration.setAutoStart(_videoConfig.autoStart);
        videoConfiguration.setSessionTimeout(_videoConfig.sessionTimeout);

        imediaManagement->setVideoEncoderConfiguration(&videoConfiguration);
        return true;
    }

    bool refreshDeviceCapabilities() {
        QScopedPointer<ONVIF::Capabilities> capabilitiesDevice(
            ideviceManagement->getCapabilitiesDevice());
        if (!capabilitiesDevice)
            return false;

        auto& src = capabilitiesDevice;
        auto& des = idata.capabilities;

        des.accessPolicyConfig     = src->accessPolicyConfig();
        des.deviceXAddr            = src->deviceXAddr();
        des.iPFilter               = src->iPFilter();
        des.zeroConfiguration      = src->zeroConfiguration();
        des.iPVersion6             = src->iPVersion6();
        des.dynDNS                 = src->dynDNS();
        des.discoveryResolve       = src->discoveryResolve();
        des.systemLogging          = src->systemLogging();
        des.firmwareUpgrade        = src->firmwareUpgrade();
        des.major                  = src->major();
        des.minor                  = src->minor();
        des.httpFirmwareUpgrade    = src->httpFirmwareUpgrade();
        des.httpSystemBackup       = src->httpSystemBackup();
        des.httpSystemLogging      = src->httpSystemLogging();
        des.httpSupportInformation = src->httpSupportInformation();
        des.inputConnectors        = src->inputConnectors();
        des.relayOutputs           = src->relayOutputs();
        des.tls11                  = src->tls11();
        des.tls22                  = src->tls22();
        des.onboardKeyGeneration   = src->onboardKeyGeneration();
        des.accessPolicyConfig     = src->accessPolicyConfig();
        des.x509Token              = src->x509Token();
        des.samlToken              = src->samlToken();
        des.kerberosToken          = src->kerberosToken();
        des.relToken               = src->relToken();
        des.tls10                  = src->tls10();
        des.dot1x                  = src->dot1x();
        des.remoteUserHanding      = src->remoteUserHanding();
        des.systemBackup           = src->systemBackup();
        des.discoveryBye           = src->discoveryBye();
        des.remoteDiscovery        = src->remoteDiscovery();

        // ptz capabilities
        QScopedPointer<ONVIF::Capabilities> capabilitiesPtz(
            ideviceManagement->getCapabilitiesDevice());
        des.ptzAddress = capabilitiesPtz->ptzXAddr();

        // image capabilities
        QScopedPointer<ONVIF::Capabilities> capabilitiesImage(
            ideviceManagement->getCapabilitiesDevice());
        des.imagingXAddress = capabilitiesImage->imagingXAddr();

        // media capabilities
        QScopedPointer<ONVIF::Capabilities> capabilitiesMedia(
            ideviceManagement->getCapabilitiesDevice());

        des.mediaXAddress = capabilitiesMedia->mediaXAddr();
        des.rtpMulticast  = capabilitiesMedia->rtpMulticast();
        des.rtpTcp        = capabilitiesMedia->rtpTcp();
        des.rtpRtspTcp    = capabilitiesMedia->rtpRtspTcp();

        return true;
    }

    bool refreshDeviceInformation() { // todo
        QHash<QString, QString> deviceInformationHash =
            ideviceManagement->getDeviceInformation();
        idata.information.manufacturer = deviceInformationHash.value("mf");
        idata.information.model        = deviceInformationHash.value("model");
        idata.information.firmwareVersion =
            deviceInformationHash.value("firmware_version");
        idata.information.serialNumber =
            deviceInformationHash.value("serial_number");
        idata.information.hardwareId =
            deviceInformationHash.value("hardware_id");
        return true;
    }

    bool refreshDeviceScopes() {
        QHash<QString, QString> deviceScopesHash =
            ideviceManagement->getDeviceScopes();
        idata.scopes.name     = deviceScopesHash.value("name");
        idata.scopes.location = deviceScopesHash.value("location");
        idata.scopes.hardware = deviceScopesHash.value("hardware");
        return true;
    }

    bool resetFactoryDevice() {
        // todo: reset factory device
        return true;
    }

    bool rebootDevice() {
        // todo: reboot device
        return true;
    }

    bool refreshVideoConfigs() {
        // get video encoder config
        QScopedPointer<ONVIF::VideoEncoderConfigurations>
            videoEncoderConfigurations(
                imediaManagement->getVideoEncoderConfigurations());

        if (!videoEncoderConfigurations)
            return false;
        {
            auto& des = idata.mediaConfig.video.encodingConfigs;
            auto& src = videoEncoderConfigurations;

            des.autoStart        = src->getAutoStart();
            des.bitrateLimit     = src->getBitrateLimit();
            des.encoding         = src->getEncoding();
            des.encodingInterval = src->getEncodingInterval();
            des.frameRateLimit   = src->getFrameRateLimit();
            des.govLength        = src->getGovLength();
            des.h264Profile      = src->getH264Profile();
            des.width            = src->getWidth();
            des.height           = src->getHeight();
            des.ipv4Address      = src->getIpv4Address();
            des.ipv6Address      = src->getIpv6Address();
            des.name             = src->getName();
            des.port             = src->getPort();
            des.quality          = src->getQuality();
            des.sessionTimeout   = src->getSessionTimeout();
            des.token            = src->getToken();
            des.ttl              = src->getTtl();
            des.type             = src->getType();
            des.useCount         = src->getUseCount();
        }

        // get video source config
        QScopedPointer<ONVIF::VideoSourceConfigurations>
            videoSourceConfigurations(
                imediaManagement->getVideoSourceConfigurations());

        if (!videoSourceConfigurations)
            return false;

        {
            auto& des       = idata.mediaConfig.video.sourceConfig;
            auto& src       = videoSourceConfigurations;
            des.name        = src->getName();
            des.useCount    = src->getUseCount();
            des.sourceToken = src->getSourceToken();
            des.bounds      = src->getBounds();
        }

        return true;
    }

    bool refreshStreamUri() {
        // get video stream uri
        QScopedPointer<ONVIF::StreamUri> streamUri(
            imediaManagement->getStreamUri(
                idata.profiles.toKenPro.value(0))); // todo: whats this input
        // exactly? meld retrun value
        // and value of odm
        if (!streamUri)
            return false;

        idata.mediaConfig.video.streamUri.uri = streamUri->uri();
        idata.mediaConfig.video.streamUri.invalidAfterConnect =
            streamUri->invalidAfterConnect();
        idata.mediaConfig.video.streamUri.invalidAfterReboot =
            streamUri->invalidAfterReboot();
        idata.mediaConfig.video.streamUri.timeout = streamUri->timeout();
        return true;
    }

    bool refreshVideoConfigsOptions() {
        idata.mediaConfig.video.encodingConfigs.options.clear();

        // get video encoder options
        foreach (
            QString _configToken,
            idata.mediaConfig.video.encodingConfigs.token) {

            QScopedPointer<ONVIF::VideoEncoderConfigurationOptions>
                videoEncoderConfigurationOptions(
                    imediaManagement->getVideoEncoderConfigurationOptions(
                        _configToken, ""));

            if (!videoEncoderConfigurationOptions)
                return false;

            Data::MediaConfig::Video::EncoderConfigs::Option encodingOptions;

            auto& des = encodingOptions;
            auto& src = videoEncoderConfigurationOptions;

            des.encodingIntervalRangeMaxH264 =
                src->encodingIntervalRangeMaxH264();
            des.encodingIntervalRangeMinH264 =
                src->encodingIntervalRangeMinH264();
            des.frameRateRangeMaxH264  = src->frameRateRangeMaxH264();
            des.frameRateRangeMinH264  = src->frameRateRangeMinH264();
            des.bitRateRangeMax        = src->bitRateRangeMax();
            des.bitRateRangeMin        = src->bitRateRangeMin();
            des.govLengthRangeMax      = src->govLengthRangeMax();
            des.govLengthRangeMin      = src->govLengthRangeMin();
            des.qualityRangeMin        = src->qualityRangeMin();
            des.qualityRangeMax        = src->qulityRangeMax();
            des.resAvailableHeightH264 = src->resAvailableHeightH264();
            des.resAvailableWidthH264  = src->resAvailableWidthH264();
            des.encodingIntervalRangeMaxJpeg =
                src->encodingIntervalRangeMaxJpeg();
            des.encodingIntervalRangeMinJpeg =
                src->encodingIntervalRangeMinJpeg();
            des.frameRateRangeMaxJpeg  = src->frameRateRangeMaxJpeg();
            des.frameRateRangeMinJpeg  = src->frameRateRangeMinJpeg();
            des.resAvailableHeightJpeg = src->resAvailableHeightJpeg();
            des.resAvailableWidthJpeg  = src->resAvailableWidthJpeg();

            foreach (
                ONVIF::VideoEncoderConfigurationOptions::H264ProfilesSupported
                    h264ProfilesSupporte,
                src->getH264ProfilesSupported()) {
                int intCastTemp = static_cast<int>(h264ProfilesSupporte);

                Data::MediaConfig::Video::EncoderConfigs::Option::
                    H264ProfilesSupported enumCastTemp =
                        static_cast<Data::MediaConfig::Video::EncoderConfigs::
                                        Option::H264ProfilesSupported>(
                            intCastTemp);

                des.h264ProfilesSupported.append(enumCastTemp);
            }
            idata.mediaConfig.video.encodingConfigs.options.append(des);
        }

        return true;
    }

    bool refreshAudioConfigs() {
        // todo: add giving audio options of cameras
        imediaManagement->getAudioEncoderConfigurationOptions();
        imediaManagement->getAudioEncoderConfigurations();
        imediaManagement->getAudioSourceConfigurations();
        return true;
    }

    bool refreshProfiles() {
        QScopedPointer<ONVIF::Profiles> profiles(
            imediaManagement->getProfiles());
        if (!profiles)
            return false;
        {
            auto& des = idata.profiles;

            des.analytics           = profiles->m_analytics;
            des.toKenPro            = profiles->m_toKenPro;
            des.fixed               = profiles->m_fixed;
            des.namePro             = profiles->m_namePro;
            des.nameVsc             = profiles->m_nameVsc;
            des.useCountVsc         = profiles->m_useCountVsc;
            des.sourceTokenVsc      = profiles->m_sourceTokenVsc;
            des.boundsVsc           = profiles->m_boundsVsc;
            des.nameVec             = profiles->m_nameVec;
            des.useCountVec         = profiles->m_useCountVec;
            des.encodingVec         = profiles->m_encodingVec;
            des.widthVec            = profiles->m_widthVec;
            des.heightVec           = profiles->m_heightVec;
            des.qualityVec          = profiles->m_qualityVec;
            des.frameRateLimitVec   = profiles->m_frameRateLimitVec;
            des.encodingIntervalVec = profiles->m_encodingIntervalVec;
            des.bitrateLimitVec     = profiles->m_bitrateLimitVec;
            des.govLengthVec        = profiles->m_govLengthVec;
            des.h264ProfileVec      = profiles->m_h264ProfileVec;
            des.typeVec             = profiles->m_typeVec;
            des.ipv4AddressVec      = profiles->m_ipv4AddressVec;
            des.ipv6AddressVec      = profiles->m_ipv6AddressVec;
            des.portVec             = profiles->m_portVec;
            des.ttlVec              = profiles->m_ttlVec;
            des.autoStartVec        = profiles->m_autoStartVec;
            des.sessionTimeoutVec   = profiles->m_sessionTimeoutVec;
            des.namePtz             = profiles->m_namePtz;
            des.useCountPtz         = profiles->m_useCountPtz;
            des.nodeToken           = profiles->m_nodeToken;
            des.panTiltSpace        = profiles->m_panTiltSpace;
            des.panTiltX            = profiles->m_panTiltX;
            des.panTiltY            = profiles->m_panTiltY;
            des.zoomSpace           = profiles->m_zoomSpace;
            des.zoomX               = profiles->m_zoomX;
            des.defaultPTZTimeout   = profiles->m_defaultPTZTimeout;
            des.panTiltUri          = profiles->m_panTiltUri;
            des.xRangeMinPt         = profiles->m_xRangeMinPt;
            des.xRangeMaxPt         = profiles->m_xRangeMaxPt;
            des.yRangeMinPt         = profiles->m_yRangeMinPt;
            des.yRangeMaxPt         = profiles->m_yRangeMaxPt;
            des.zoomUri             = profiles->m_zoomUri;
            des.xRangeMinZm         = profiles->m_xRangeMinZm;
            des.xRangeMaxZm         = profiles->m_xRangeMaxZm;
            des.nameMc              = profiles->m_nameMc;
            des.useCountMc          = profiles->m_useCountMc;
            des.status              = profiles->m_status;
            des.position            = profiles->m_position;
            des.filter              = profiles->m_filter;
            des.subscriptionPolicy  = profiles->m_subscriptionPolicy;
            des.analytics           = profiles->m_analytics;
            des.typeMc              = profiles->m_typeMc;
            des.ipv4AddressMc       = profiles->m_ipv4AddressMc;
            des.ipv6AddressMc       = profiles->m_ipv6AddressMc;
            des.portMc              = profiles->m_portMc;
            des.ttlMc               = profiles->m_ttlMc;
            des.autoStartMc         = profiles->m_autoStartMc;
            des.sessionTimeoutMc    = profiles->m_sessionTimeoutMc;
            des.defaultAbsolutePantTiltPositionSpace =
                profiles->m_defaultAbsolutePantTiltPositionSpace;
            des.defaultAbsoluteZoomPositionSpace =
                profiles->m_defaultAbsoluteZoomPositionSpace;
            des.defaultRelativePantTiltTranslationSpace =
                profiles->m_defaultRelativePantTiltTranslationSpace;
            des.defaultRelativeZoomTranslationSpace =
                profiles->m_defaultRelativeZoomTranslationSpace;
            des.defaultContinuousPantTiltVelocitySpace =
                profiles->m_defaultContinuousPantTiltVelocitySpace;
            des.defaultContinuousZoomVelocitySpace =
                profiles->m_defaultContinuousZoomVelocitySpace;
        }
        QScopedPointer<ONVIF::Profile> profile720p(
            imediaManagement->getProfile720P());
        if (!profile720p)
            return false;
        auto& des = idata.profile720p;

        des.analytics           = profile720p->m_analytics;
        des.toKenPro            = profile720p->m_toKenPro;
        des.fixed               = profile720p->m_fixed;
        des.namePro             = profile720p->m_namePro;
        des.nameVsc             = profile720p->m_nameVsc;
        des.useCountVsc         = profile720p->m_useCountVsc;
        des.sourceTokenVsc      = profile720p->m_sourceTokenVsc;
        des.boundsVsc           = profile720p->m_boundsVsc;
        des.nameVec             = profile720p->m_nameVec;
        des.useCountVec         = profile720p->m_useCountVec;
        des.encodingVec         = profile720p->m_encodingVec;
        des.widthVec            = profile720p->m_widthVec;
        des.heightVec           = profile720p->m_heightVec;
        des.qualityVec          = profile720p->m_qualityVec;
        des.frameRateLimitVec   = profile720p->m_frameRateLimitVec;
        des.encodingIntervalVec = profile720p->m_encodingIntervalVec;
        des.bitrateLimitVec     = profile720p->m_bitrateLimitVec;
        des.govLengthVec        = profile720p->m_govLengthVec;
        des.h264ProfileVec      = profile720p->m_h264ProfileVec;
        des.typeVec             = profile720p->m_typeVec;
        des.ipv4AddressVec      = profile720p->m_ipv4AddressVec;
        des.ipv6AddressVec      = profile720p->m_ipv6AddressVec;
        des.portVec             = profile720p->m_portVec;
        des.ttlVec              = profile720p->m_ttlVec;
        des.autoStartVec        = profile720p->m_autoStartVec;
        des.sessionTimeoutVec   = profile720p->m_sessionTimeoutVec;
        des.namePtz             = profile720p->m_namePtz;
        des.useCountPtz         = profile720p->m_useCountPtz;
        des.nodeToken           = profile720p->m_nodeToken;
        des.panTiltSpace        = profile720p->m_panTiltSpace;
        des.panTiltX            = profile720p->m_panTiltX;
        des.panTiltY            = profile720p->m_panTiltY;
        des.zoomSpace           = profile720p->m_zoomSpace;
        des.zoomX               = profile720p->m_zoomX;
        des.defaultPTZTimeout   = profile720p->m_defaultPTZTimeout;
        des.panTiltUri          = profile720p->m_panTiltUri;
        des.xRangeMinPt         = profile720p->m_xRangeMinPt;
        des.xRangeMaxPt         = profile720p->m_xRangeMaxPt;
        des.yRangeMinPt         = profile720p->m_yRangeMinPt;
        des.yRangeMaxPt         = profile720p->m_yRangeMaxPt;
        des.zoomUri             = profile720p->m_zoomUri;
        des.xRangeMinZm         = profile720p->m_xRangeMinZm;
        des.xRangeMaxZm         = profile720p->m_xRangeMaxZm;
        des.nameMc              = profile720p->m_nameMc;
        des.useCountMc          = profile720p->m_useCountMc;
        des.status              = profile720p->m_status;
        des.position            = profile720p->m_position;
        des.filter              = profile720p->m_filter;
        des.subscriptionPolicy  = profile720p->m_subscriptionPolicy;
        des.analytics           = profile720p->m_analytics;
        des.typeMc              = profile720p->m_typeMc;
        des.ipv4AddressMc       = profile720p->m_ipv4AddressMc;
        des.ipv6AddressMc       = profile720p->m_ipv6AddressMc;
        des.portMc              = profile720p->m_portMc;
        des.ttlMc               = profile720p->m_ttlMc;
        des.autoStartMc         = profile720p->m_autoStartMc;
        des.sessionTimeoutMc    = profile720p->m_sessionTimeoutMc;
        des.defaultAbsolutePantTiltPositionSpace =
            profile720p->m_defaultAbsolutePantTiltPositionSpace;
        des.defaultAbsoluteZoomPositionSpace =
            profile720p->m_defaultAbsoluteZoomPositionSpace;
        des.defaultRelativePantTiltTranslationSpace =
            profile720p->m_defaultRelativePantTiltTranslationSpace;
        des.defaultRelativeZoomTranslationSpace =
            profile720p->m_defaultRelativeZoomTranslationSpace;
        des.defaultContinuousPantTiltVelocitySpace =
            profile720p->m_defaultContinuousPantTiltVelocitySpace;
        des.defaultContinuousZoomVelocitySpace =
            profile720p->m_defaultContinuousZoomVelocitySpace;

        QScopedPointer<ONVIF::Profile> profileD1(
            imediaManagement->getProfileD1());
        if (!profileD1)
            return false;
        idata.profileD1.analytics           = profileD1->m_analytics;
        idata.profileD1.toKenPro            = profileD1->m_toKenPro;
        idata.profileD1.fixed               = profileD1->m_fixed;
        idata.profileD1.namePro             = profileD1->m_namePro;
        idata.profileD1.nameVsc             = profileD1->m_nameVsc;
        idata.profileD1.useCountVsc         = profileD1->m_useCountVsc;
        idata.profileD1.sourceTokenVsc      = profileD1->m_sourceTokenVsc;
        idata.profileD1.boundsVsc           = profileD1->m_boundsVsc;
        idata.profileD1.nameVec             = profileD1->m_nameVec;
        idata.profileD1.useCountVec         = profileD1->m_useCountVec;
        idata.profileD1.encodingVec         = profileD1->m_encodingVec;
        idata.profileD1.widthVec            = profileD1->m_widthVec;
        idata.profileD1.heightVec           = profileD1->m_heightVec;
        idata.profileD1.qualityVec          = profileD1->m_qualityVec;
        idata.profileD1.frameRateLimitVec   = profileD1->m_frameRateLimitVec;
        idata.profileD1.encodingIntervalVec = profileD1->m_encodingIntervalVec;
        idata.profileD1.bitrateLimitVec     = profileD1->m_bitrateLimitVec;
        idata.profileD1.govLengthVec        = profileD1->m_govLengthVec;
        idata.profileD1.h264ProfileVec      = profileD1->m_h264ProfileVec;
        idata.profileD1.typeVec             = profileD1->m_typeVec;
        idata.profileD1.ipv4AddressVec      = profileD1->m_ipv4AddressVec;
        idata.profileD1.ipv6AddressVec      = profileD1->m_ipv6AddressVec;
        idata.profileD1.portVec             = profileD1->m_portVec;
        idata.profileD1.ttlVec              = profileD1->m_ttlVec;
        idata.profileD1.autoStartVec        = profileD1->m_autoStartVec;
        idata.profileD1.sessionTimeoutVec   = profileD1->m_sessionTimeoutVec;
        idata.profileD1.namePtz             = profileD1->m_namePtz;
        idata.profileD1.useCountPtz         = profileD1->m_useCountPtz;
        idata.profileD1.nodeToken           = profileD1->m_nodeToken;
        idata.profileD1.panTiltSpace        = profileD1->m_panTiltSpace;
        idata.profileD1.panTiltX            = profileD1->m_panTiltX;
        idata.profileD1.panTiltY            = profileD1->m_panTiltY;
        idata.profileD1.zoomSpace           = profileD1->m_zoomSpace;
        idata.profileD1.zoomX               = profileD1->m_zoomX;
        idata.profileD1.defaultPTZTimeout   = profileD1->m_defaultPTZTimeout;
        idata.profileD1.panTiltUri          = profileD1->m_panTiltUri;
        idata.profileD1.xRangeMinPt         = profileD1->m_xRangeMinPt;
        idata.profileD1.xRangeMaxPt         = profileD1->m_xRangeMaxPt;
        idata.profileD1.yRangeMinPt         = profileD1->m_yRangeMinPt;
        idata.profileD1.yRangeMaxPt         = profileD1->m_yRangeMaxPt;
        idata.profileD1.zoomUri             = profileD1->m_zoomUri;
        idata.profileD1.xRangeMinZm         = profileD1->m_xRangeMinZm;
        idata.profileD1.xRangeMaxZm         = profileD1->m_xRangeMaxZm;
        idata.profileD1.nameMc              = profileD1->m_nameMc;
        idata.profileD1.useCountMc          = profileD1->m_useCountMc;
        idata.profileD1.status              = profileD1->m_status;
        idata.profileD1.position            = profileD1->m_position;
        idata.profileD1.filter              = profileD1->m_filter;
        idata.profileD1.subscriptionPolicy  = profileD1->m_subscriptionPolicy;
        idata.profileD1.analytics           = profileD1->m_analytics;
        idata.profileD1.typeMc              = profileD1->m_typeMc;
        idata.profileD1.ipv4AddressMc       = profileD1->m_ipv4AddressMc;
        idata.profileD1.ipv6AddressMc       = profileD1->m_ipv6AddressMc;
        idata.profileD1.portMc              = profileD1->m_portMc;
        idata.profileD1.ttlMc               = profileD1->m_ttlMc;
        idata.profileD1.autoStartMc         = profileD1->m_autoStartMc;
        idata.profileD1.sessionTimeoutMc    = profileD1->m_sessionTimeoutMc;
        idata.profileD1.defaultAbsolutePantTiltPositionSpace =
            profileD1->m_defaultAbsolutePantTiltPositionSpace;
        idata.profileD1.defaultAbsoluteZoomPositionSpace =
            profileD1->m_defaultAbsoluteZoomPositionSpace;
        idata.profileD1.defaultRelativePantTiltTranslationSpace =
            profileD1->m_defaultRelativePantTiltTranslationSpace;
        idata.profileD1.defaultRelativeZoomTranslationSpace =
            profileD1->m_defaultRelativeZoomTranslationSpace;
        idata.profileD1.defaultContinuousPantTiltVelocitySpace =
            profileD1->m_defaultContinuousPantTiltVelocitySpace;
        idata.profileD1.defaultContinuousZoomVelocitySpace =
            profileD1->m_defaultContinuousZoomVelocitySpace;
        return true;
    }

    bool refreshInterfaces() {
        QScopedPointer<ONVIF::NetworkInterfaces> networkInterfaces(
            ideviceManagement->getNetworkInterfaces());
        if (!networkInterfaces)
            return false;

        auto& des = idata.network.interfaces;
        auto& src = networkInterfaces;

        des.networkInfacesEnabled    = src->networkInfacesEnabled();
        des.autoNegotiation          = src->autoNegotiation();
        des.speed                    = src->speed();
        des.mtu                      = src->mtu();
        des.ipv4Enabled              = src->ipv4Enabled();
        des.ipv4ManualAddress        = src->ipv4ManualAddress();
        des.ipv4ManualPrefixLength   = src->ipv4ManualPrefixLength();
        des.ipv4DHCP                 = src->getIpv4DHCP();
        des.networkInfacesName       = src->networkInfacesName();
        des.hwAaddress               = src->hwAaddress();
        des.ipv4LinkLocalAddress     = src->ipv4LinkLocalAddress();
        des.ipvLinkLocalPrefixLength = src->ipvLinkLocalPrefixLength();
        des.ipv4FromDHCPAddress      = src->ipv4FromDHCPAddress();
        des.ipv4FromDHCPPrefixLength = src->ipv4FromDHCPPrefixLength();
        des.result                   = src->result();
        des.duplex = static_cast<Data::Network::Interfaces::Duplex>(
            static_cast<int>(src->duplex()));

        return true;
    }

    bool refreshUsers() {
        QScopedPointer<ONVIF::Users> users(ideviceManagement->getUsers());
        if (!users)
            return false;
        idata.users.username  = users->userName();
        idata.users.password  = users->passWord();
        idata.users.userLevel = static_cast<Data::Users::UserLevelType>(
            static_cast<int>(users->userLevel()));
        return true;
    }
    bool refreshPtzConfiguration() {
        ONVIF::Configuration* config = new ONVIF::Configuration;
        iptzManagement->getConfiguration(config);
        auto& des = idata.ptz.config;

        des.name                  = config->name();
        des.useCount              = config->useCount();
        des.nodeToken             = config->nodeToken();
        des.panTiltY              = config->panTiltX();
        des.panTiltY              = config->panTiltY();
        des.zoomSpace             = config->zoomSpace();
        des.defaultPTZTimeout     = config->defaultPTZTimeout();
        des.panTiltUri            = config->panTiltUri();
        des.panTiltXRangeMin      = config->panTiltXRangeMin();
        des.panTiltXRangeMax      = config->panTiltXRangeMax();
        des.panTiltYRangeMin      = config->panTiltXRangeMin();
        des.panTiltYRangeMax      = config->panTiltYRangeMax();
        des.zoomUri               = config->zoomUri();
        des.zoomXRangeMin         = config->zoomXRangeMin();
        des.zoomXRangeMax         = config->zoomXRangeMax();
        des.ptzConfigurationToken = config->ptzConfigurationToken();
        des.panTiltSpace          = config->panTiltSpace();
        des.zoomX                 = config->zoomX();

        des.defaultAbsolutePantTiltPositionSpace =
            config->defaultAbsolutePantTiltPositionSpace();
        des.defaultAbsoluteZoomPositionSpace =
            config->defaultAbsoluteZoomPositionSpace();
        des.defaultRelativePanTiltTranslationSpace =
            config->defaultRelativePanTiltTranslationSpace();
        des.defaultRelativeZoomTranslationSpace =
            config->defaultRelativeZoomTranslationSpace();
        des.defaultContinuousPanTiltVelocitySpace =
            config->defaultContinuousPanTiltVelocitySpace();

        delete config;
        return true;
    }
};

// QOnvifDevice::QOnvifDevice() {}

QOnvifDevice::QOnvifDevice(
    QString  _serviceAddress,
    QString  _userName,
    QString  _password,
    QObject* _parent)
    : d_ptr{new QOnvifDevicePrivate{_serviceAddress, _userName, _password}},
      QObject(_parent) {}

QOnvifDevice::~QOnvifDevice() {}

Data&
QOnvifDevice::data() {
    return d_ptr->idata;
}

bool
QOnvifDevice::deviceDateAndTime(Data::DateTime& _datetime) {
    return d_ptr->deviceDateAndTime(_datetime);
}

void
QOnvifDevice::setDeviceProbeData(Data::ProbeData _probeData) {
    d_ptr->setDeviceProbeData(_probeData);
}

bool
QOnvifDevice::setScopes(QString _name, QString _location) {
    return d_ptr->setScopes(_name, _location);
}

bool
QOnvifDevice::setVideoConfig(
    Data::MediaConfig::Video::EncoderConfig _videoConfig) {
    return d_ptr->setVideoConfig(_videoConfig);
}

bool
QOnvifDevice::setDateAndTime(QDateTime _dateAndTime) {
    return d_ptr->setDeviceDateAndTime(_dateAndTime);
}

bool
QOnvifDevice::refreshDeviceCapabilities() {
    return d_ptr->refreshDeviceCapabilities();
}

bool
QOnvifDevice::refreshDeviceInformation() {
    return d_ptr->refreshDeviceInformation();
}

bool
QOnvifDevice::refreshDeviceScopes() {
    return d_ptr->refreshDeviceScopes();
}

bool
QOnvifDevice::resetFactoryDevice() {
    return d_ptr->resetFactoryDevice();
}

bool
QOnvifDevice::rebootDevice() {
    return d_ptr->rebootDevice();
}

bool
QOnvifDevice::refreshVideoConfigs() {
    bool result = d_ptr->refreshVideoConfigs();
    refreshVideoConfigsOptions();
    return result;
}

bool
QOnvifDevice::refreshVideoConfigsOptions() {
    return d_ptr->refreshVideoConfigsOptions();
}

bool
QOnvifDevice::refreshStreamUri() {
    return d_ptr->refreshStreamUri();
}

bool
QOnvifDevice::refreshAudioConfigs() {
    return d_ptr->refreshAudioConfigs();
}

bool
QOnvifDevice::refreshProfiles() {
    return d_ptr->refreshProfiles();
}

bool
QOnvifDevice::refreshInterfaces() {
    return d_ptr->refreshInterfaces();
}

bool
QOnvifDevice::refreshUsers() {
    return d_ptr->refreshUsers();
}

bool
QOnvifDevice::refreshPtzConfiguration() {
    return d_ptr->refreshPtzConfiguration();
}

///////////////////////////////////////////////////////////////////////////////
} // namespace device
///////////////////////////////////////////////////////////////////////////////
