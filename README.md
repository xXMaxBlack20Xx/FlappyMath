# FlappyMath Refactorizado

## Objetivo
Convertir tu juego estructurado en un sistema modular, reutilizable, escalable y mantenible aplicando:

- **POO en C++**  
- **Patrones de diseño** (Singleton, Factory, State, etc.)  
- **Buenas prácticas** (separación de responsabilidades, manejo de recursos, modularización)  

---

## Plan General de Refactorización

### 1. Organización de Archivos por Módulo  
Reorganiza en carpetas como:


---

### 2. Clases Principales (POO)

| Clase          | Descripción                                                      |
| -------------- | ---------------------------------------------------------------- |
| `Game`         | Controla el ciclo principal del juego. Implementa Singleton.     |
| `Player`       | Representa al jugador: movimiento, colisiones y sprite.          |
| `Pipe`         | Cada tubería: lógica de colisión y movimiento.                   |
| `Operation`    | Genera y evalúa operaciones matemáticas dinámicamente.           |
| `ScoreManager` | Guarda y carga puntuaciones (Singleton para persistencia).       |
| `UIManager`    | Dibuja pantallas de inicio, game over y HUD de puntuación.       |
| `SoundManager` | Reproduce sonidos y música de fondo (Singleton).                 |

---

### 3. Aplicar Patrones de Diseño

| Patrón      | Uso en el juego                                                        |
| ----------- | ---------------------------------------------------------------------- |
| **Singleton** | `Game`, `SoundManager`, `ScoreManager` para instancias únicas globales. |
| **Factory**   | `Operation::Generate()` decide y construye la operación matemática.    |
| **State**     | Gestiona estados del juego: _Inicio_, _Jugando_, _GameOver_.          |
| **Strategy** (opcional) | Permite cambiar dinámicamente la lógica de generación de operaciones. |

---
