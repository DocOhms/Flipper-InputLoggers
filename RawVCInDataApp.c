#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <furi_hal_gpio.h>
#include <furi_hal_adc.h>
#include <storage/storage.h>

#define GPIO_VOLTAGE_PIN FURI_HAL_GPIO_PIN_XXX // Replace with actual pin
#define GPIO_CURRENT_PIN FURI_HAL_GPIO_PIN_YYY // Replace with actual pin

typedef struct {
    float voltage;
    float current;
} Measurement;

static void draw_measurements(Canvas* canvas, Measurement* measurement) {
    char buffer[64];

    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);

    snprintf(buffer, sizeof(buffer), "Voltage: %.2f V", measurement->voltage);
    canvas_draw_str_aligned(canvas, 64, 20, AlignCenter, AlignTop, buffer);

    snprintf(buffer, sizeof(buffer), "Current: %.2f A", measurement->current);
    canvas_draw_str_aligned(canvas, 64, 40, AlignCenter, AlignTop, buffer);

    canvas_draw_str_aligned(canvas, 64, 60, AlignCenter, AlignTop, "Press Center to Record");
}

static void record_to_csv(const Measurement* measurement) {
    Storage* storage = furi_record_open("storage");
    StorageFile* file = storage_file_alloc(storage);

    if (storage_file_open(file, "/ext/measurements.csv", FSAM_WRITE_APPEND, FSOM_OPEN_ALWAYS)) {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "%.2f,%.2f\n", measurement->voltage, measurement->current);
        storage_file_write(file, buffer, strlen(buffer));
        storage_file_close(file);
    }

    storage_file_free(file);
    furi_record_close("storage");
}

static float read_voltage() {
    return furi_hal_adc_read(GPIO_VOLTAGE_PIN); // Adjust reading logic for actual ADC conversion
}

static float read_current() {
    return furi_hal_adc_read(GPIO_CURRENT_PIN); // Adjust reading logic for actual ADC conversion
}

int32_t gpio_measurement_app() {
    ViewPort* view_port = view_port_alloc();
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    Measurement measurement = {0.0f, 0.0f};

    bool running = true;

    while (running) {
        measurement.voltage = read_voltage();
        measurement.current = read_current();

        view_port_draw_callback_set(view_port, (ViewPortDrawCallback)draw_measurements, &measurement);
        view_port_update(view_port);

        InputEvent event;
        if (furi_hal_input_read(&event, 100)) {
            if (event.type == InputTypePress && event.key == InputKeyOk) {
                record_to_csv(&measurement);
            } else if (event.key == InputKeyBack) {
                running = false;
            }
        }
    }

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close("gui");

    return 0;
}

