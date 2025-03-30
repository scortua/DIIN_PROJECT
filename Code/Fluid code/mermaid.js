---
config:
  theme: neo
  themeVariables:
    primaryColor: '#ffdfd3'
    edgeLabelBackground: '#fff'
  layout: elk
  look: neo
---
flowchart TB
 subgraph ESP32_1["ESP32 #1 (Red Neuronal + Sensores)"]
        B{{"Cámara/Micrófono activo?"}}
        A["Inicio"]
        C["Capturar imagen de planta"]
        D["Preprocesar imagen"]
        E["Red Neuronal clasifica: Especie Salud Necesidades"]
        F["Capturar comando de voz"]
        G["Procesar audio Extraer acción"]
  end
 subgraph ESP32_2["ESP32 #2 (Actuadores + Web)"]
        H{"¿Requiere acción?"}
        I{{"Tipo de acción:"}}
        J["Activar electroimán (Seleccionar boquilla)"]
        K["Posicionar cordenadas X-Y-Z"]
        L["Mover motores paso a paso (sincronización RTOS)"]
        M["Ejecutar secuencia: Cambiar boquilla. Posicionar. Acción"]
        N["Registrar datos en log"]
        O["Actualizar interfaz web: Estado actual-Gráficos-Alertas"]
  end
 subgraph Interfaz_Web["Interfaz Web"]
        P["Mostrar en tiempo real: Video/Imágenes Sensores Posición actuadores"]
        Q["Botones interactivos: Control manual Prioridades"]
        R["Envía comandos → ESP32 #2"]
  end
    A --> B
    B -- Cámara --> C
    C --> D
    D --> E
    B -- Micrófono --> F
    F --> G
    E --> H
    G --> H
    H -- Sí --> I
    I -- Cambio boquilla --> J
    I -- Posicionamiento --> K
    K --> L
    J --> M
    H -- No --> N
    M --> O & B
    N --> O & B
    O --> P
    P --> Q
    Q --> R
    R --> H
    L --> O
    style ESP32_1 fill:#e3f2fd,stroke:#2196f3
    style ESP32_2 fill:#e8f5e9,stroke:#4caf50
    style Interfaz_Web fill:#fff3e0,stroke:#ff9800
