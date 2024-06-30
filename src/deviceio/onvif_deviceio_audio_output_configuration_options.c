#include "onvif_deviceio_audio_output_configuration_options_local.h"
#include "clogger.h"

typedef struct {
    char ** outputTokensAvailable;
    int token_count;
    
    char ** sendPrimacyOptions;
    int primacy_count;

    int output_min;
    int output_max;
} OnvifDeviceIOAudioOutputConfigurationOptionsPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(OnvifDeviceIOAudioOutputConfigurationOptions, OnvifDeviceIOAudioOutputConfigurationOptions_, SOAP_TYPE_OBJECT)

static void
OnvifDeviceIOAudioOutputConfigurationOptions__reset (OnvifDeviceIOAudioOutputConfigurationOptions *self);

static void
OnvifDeviceIOAudioOutputConfigurationOptions__construct(SoapObject * obj, gpointer ptr){
    struct _tmd__GetAudioOutputConfigurationOptionsResponse * response = ptr;
    OnvifDeviceIOAudioOutputConfigurationOptions * self = ONVIF_DEVICEIO_AUDIO_OUTPUT_CONFIGURATION_OPTIONS(obj);
    OnvifDeviceIOAudioOutputConfigurationOptionsPrivate *priv = OnvifDeviceIOAudioOutputConfigurationOptions__get_instance_private (self);

    OnvifDeviceIOAudioOutputConfigurationOptions__reset(self);
    if(!response || !response->AudioOutputOptions){
        return;
    }

    priv->token_count = response->AudioOutputOptions->__sizeOutputTokensAvailable;
    priv->outputTokensAvailable = malloc(sizeof(char*) * priv->token_count);
    for(int i=0;i<priv->token_count;i++){
        priv->outputTokensAvailable[i] = malloc(strlen(response->AudioOutputOptions->OutputTokensAvailable[i]) +1);
        strcpy(priv->outputTokensAvailable[i],response->AudioOutputOptions->OutputTokensAvailable[i]);
    }

    priv->primacy_count = response->AudioOutputOptions->__sizeSendPrimacyOptions;
    priv->sendPrimacyOptions = malloc(sizeof(char*) * priv->primacy_count);
    for(int i=0;i<priv->primacy_count;i++){
        priv->sendPrimacyOptions[i] = malloc(strlen(response->AudioOutputOptions->SendPrimacyOptions[i]) +1);
        strcpy(priv->sendPrimacyOptions[i],response->AudioOutputOptions->SendPrimacyOptions[i]);
    }

    priv->output_min = response->AudioOutputOptions->OutputLevelRange->Min;
    priv->output_max = response->AudioOutputOptions->OutputLevelRange->Max;
}

static void
OnvifDeviceIOAudioOutputConfigurationOptions__reset (OnvifDeviceIOAudioOutputConfigurationOptions *self){
    OnvifDeviceIOAudioOutputConfigurationOptionsPrivate *priv = OnvifDeviceIOAudioOutputConfigurationOptions__get_instance_private (self);
    
    if(priv->outputTokensAvailable){
        free(priv->outputTokensAvailable);
        priv->outputTokensAvailable = NULL;
        priv->token_count = 0;
    }

    if(priv->sendPrimacyOptions){
        free(priv->sendPrimacyOptions);
        priv->sendPrimacyOptions = NULL;
        priv->primacy_count = 0;
    }

    priv->output_min = 0;
    priv->output_max = 0;
}

static void
OnvifDeviceIOAudioOutputConfigurationOptions__dispose (GObject *self)
{
    OnvifDeviceIOAudioOutputConfigurationOptions__reset(ONVIF_DEVICEIO_AUDIO_OUTPUT_CONFIGURATION_OPTIONS(self));

    G_OBJECT_CLASS (OnvifDeviceIOAudioOutputConfigurationOptions__parent_class)->dispose (self);
}

static void
OnvifDeviceIOAudioOutputConfigurationOptions__class_init (OnvifDeviceIOAudioOutputConfigurationOptionsClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->dispose = OnvifDeviceIOAudioOutputConfigurationOptions__dispose;
    SoapObjectClass *soapobj_class = SOAP_OBJECT_CLASS(klass);
    soapobj_class->construct = OnvifDeviceIOAudioOutputConfigurationOptions__construct;
}

static void
OnvifDeviceIOAudioOutputConfigurationOptions__init (OnvifDeviceIOAudioOutputConfigurationOptions * self)
{
    OnvifDeviceIOAudioOutputConfigurationOptionsPrivate *priv = OnvifDeviceIOAudioOutputConfigurationOptions__get_instance_private (ONVIF_DEVICEIO_AUDIO_OUTPUT_CONFIGURATION_OPTIONS(self));
    priv->outputTokensAvailable = NULL;
    priv->token_count = 0;
    priv->sendPrimacyOptions = NULL;
    priv->primacy_count = 0;
    priv->output_min = 0;
    priv->output_max = 0;
}

OnvifDeviceIOAudioOutputConfigurationOptions* OnvifDeviceIOAudioOutputConfigurationOptions__new(struct _tmd__GetAudioOutputConfigurationOptionsResponse * resp){
    return g_object_new (ONVIF_TYPE_DEVICEIO_AUDIO_OUTPUT_CONFIGURATION_OPTIONS, "data", resp, NULL);
}

//TODO Implement getters