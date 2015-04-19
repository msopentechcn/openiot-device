#include "sitewhere.h"
#include "sitewhere.pb.h"
#include "pb_encode.h"
/**#include "pb_decode.h"*/
#include "double_conversion.h"

// Signals end of stream.
uint8_t zero = 0;

unsigned int sw_register(char* hardwareId, char* specificationToken, uint8_t* buffer, size_t length, char* originator) {
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, length);

    SiteWhere_Header header = { };
    header.command = SiteWhere_Command_REGISTER;
    if (originator != NULL) {
        header.has_originator = true;
        strcpy(header.originator, originator);
    }
    if (!pb_encode_delimited(&stream, SiteWhere_Header_fields, &header)) {
        return 0;
    }

    SiteWhere_RegisterDevice registerDevice = { };
    strcpy(registerDevice.hardwareId, hardwareId);
    strcpy(registerDevice.specificationToken, specificationToken);
    if (!pb_encode_delimited(&stream, SiteWhere_RegisterDevice_fields, &registerDevice)) {
        return 0;
    }

    return stream.bytes_written;
}

unsigned int sw_acknowledge(char* hardwareId, char* message, uint8_t* buffer, size_t length, char* originator) {
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, length);

    SiteWhere_Header header = { };
    header.command = SiteWhere_Command_ACKNOWLEDGE;
    if (originator != NULL) {
        header.has_originator = true;
        strcpy(header.originator, originator);
    }
    if (!pb_encode_delimited(&stream, SiteWhere_Header_fields, &header)) {
        return 0;
    }

    SiteWhere_Acknowledge ack = { };
    strcpy(ack.hardwareId, hardwareId);
    if (message != NULL) {
        ack.has_message = true;
        strcpy(ack.message, message);
    }
    if (!pb_encode_delimited(&stream, SiteWhere_Acknowledge_fields, &ack)) {
        return 0;
    }

    return stream.bytes_written;
}

unsigned int sw_measurement(char* hardwareId, char* name, char* value, int64_t eventDate,
        uint8_t* buffer, size_t length, char* originator) {
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, length);

    SiteWhere_Header header = { };
    header.command = SiteWhere_Command_DEVICEMEASUREMENT;
    if (originator != NULL) {
        header.has_originator = true;
        strcpy(header.originator, originator);
    }
    if (!pb_encode_delimited(&stream, SiteWhere_Header_fields, &header)) {
        return 0;
    }


    SiteWhere_DeviceMeasurements measurements = { };
    strcpy(measurements.hardwareId, hardwareId);

    SiteWhere_Measurement measurement = { };
    strcpy(measurement.measurementId, name);
    //measurement.measurementValue = float_to_double(value);
    //measurement.measurementValue = value;
    strcpy(measurement.measurementValue, value);
    measurements.measurement[0] = measurement;
    measurements.measurement_count = 1;

    if (eventDate != 0) {
        measurements.has_eventDate = true;
        measurements.eventDate = eventDate;
    }
    if (!pb_encode_delimited(&stream, SiteWhere_DeviceMeasurements_fields, &measurements)) {
        return 0;
    }

    return stream.bytes_written;
}

unsigned int sw_measurement_multi(char* hardwareId, char** measurementTokens, int64_t eventDate,
        uint8_t* buffer, size_t length, char* originator) {
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, length);

    SiteWhere_Header header = { };
    header.command = SiteWhere_Command_DEVICEMEASUREMENT;
    if (originator != NULL) {
        header.has_originator = true;
        strcpy(header.originator, originator);
    }
    if (!pb_encode_delimited(&stream, SiteWhere_Header_fields, &header)) {
        return 0;
    }

    SiteWhere_DeviceMeasurements measurements = { };
    strcpy(measurements.hardwareId, hardwareId);

    measurements.measurement_count = 0;
    if (measurementTokens) {
        int i;
        for (i = 0; *(measurementTokens + i); i++){
            SiteWhere_Measurement measurement = { };
            char * key =  strtok(*(measurementTokens + i), ":");
            //float value = atof(strtok(NULL, ":"));
            char * value = strtok(NULL, ":");
    //printf("value is %lf\n", value);
            strcpy(measurement.measurementId, key);
            //measurement.measurementValue = float_to_double(value);
            //measurement.measurementValue = value;
            strcpy(measurement.measurementValue, value);
            measurements.measurement[i] = measurement;
            measurements.measurement_count++;
            free(*(measurementTokens + i));
        }
        free(measurementTokens);
    }

    if (eventDate != 0) {
        measurements.has_eventDate = true;
        measurements.eventDate = eventDate;
    }
    if (!pb_encode_delimited(&stream, SiteWhere_DeviceMeasurements_fields, &measurements)) {
        return 0;
    }

    return stream.bytes_written;
}


unsigned int sw_location(char* hardwareId, char* lat, char* lon, char* ele, int64_t eventDate,
        uint8_t* buffer, size_t length, char* originator) {
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, length);

    SiteWhere_Header header = { };
    header.command = SiteWhere_Command_DEVICELOCATION;
    if (originator != NULL) {
        header.has_originator = true;
        strcpy(header.originator, originator);
    }
    if (!pb_encode_delimited(&stream, SiteWhere_Header_fields, &header)) {
        return 0;
    }

    SiteWhere_DeviceLocation location = { };
    strcpy(location.hardwareId, hardwareId);

    strcpy(location.latitude, lat);
    strcpy(location.longitude, lon);
    strcpy(location.elevation, ele);
    location.has_elevation = true;

    if (eventDate != 0) {
        location.has_eventDate = true;
        location.eventDate = eventDate;
    }
    if (!pb_encode_delimited(&stream, SiteWhere_DeviceLocation_fields, &location)) {
        return 0;
    }

    return stream.bytes_written;
}

unsigned int sw_alert(char* hardwareId, char* alertType, char* alertMessage, int64_t eventDate,
        uint8_t* buffer, size_t length, char* originator) {
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, length);

    SiteWhere_Header header = { };
    header.command = SiteWhere_Command_DEVICEALERT;
    if (originator != NULL) {
        header.has_originator = true;
        strcpy(header.originator, originator);
    }
    if (!pb_encode_delimited(&stream, SiteWhere_Header_fields, &header)) {
        return 0;
    }

    SiteWhere_DeviceAlert alert = { };
    strcpy(alert.hardwareId, hardwareId);
    strcpy(alert.alertType, alertType);
    strcpy(alert.alertMessage, alertMessage);
    if (eventDate != 0) {
        alert.has_eventDate = true;
        alert.eventDate = eventDate;
    }
    if (!pb_encode_delimited(&stream, SiteWhere_DeviceAlert_fields, &alert)) {
        return 0;
    }

    return stream.bytes_written;
}
