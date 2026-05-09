# Electronics

Repo where I share my learnings about electronics, my projects, my designs and my code.

## Arduino Projects

- [Shelf lights](shelf_lights/README.md)
- [Garage control](garage_control/README.md)

## ESPHome Projects

- [Siren](siren/README.md)

The [secrets.yaml](secrets.yaml) file contains the placeholders for wifi and mqtt information. **IMPORTANT**: Do not upload this file with the actual secrets.

The secrets.yaml should have this structure:

```yaml
mqtt_broker: BROKER_IP
wifi_ssid: "YOUR_WIFI_SSID"
wifi_password: "YOUR_WIFI_PASSWORD"
```

## Templates

Generic, experimental C code that can be shared among different projects. Does not work as is, needs heavy adaptation.
