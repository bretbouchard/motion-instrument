#!/usr/bin/env python3
"""
generate_presets.py - Generate JUCE XML preset files for Breath Lead (Motion Instrument)

Updated for all 13 parameters:
- air, tone, formant, resistance
- vibratoDepth, vibratoRateHz
- noiseColor, sineAnchor
- motionSustain, motionSensitivity
- attackMs, releaseMs
- outputGainDb
"""

import os
import xml.etree.ElementTree as ET
from xml.dom import minidom

def create_preset(name,
                  air=0.5,
                  tone=0.6,
                  formant=0.5,
                  resistance=0.4,
                  vibratoDepth=0.0,
                  vibratoRateHz=5.0,
                  noiseColor=0.7,
                  sineAnchor=0.3,
                  motionSustain=True,
                  motionSensitivity=0.6,
                  attackMs=50.0,
                  releaseMs=300.0,
                  outputGainDb=-3.0,
                  author="SchillingerEcosystem"):
    """Create a JUCE preset XML file"""

    # Convert boolean to string
    motionSustainStr = "1" if motionSustain else "0"

    # Create root element
    root = ET.Element("PRESET")
    root.set("version", "1")
    root.set("name", name)
    root.set("author", author)
    root.set("plugin", "BreathLead")

    # Create values element
    values = ET.SubElement(root, "VALUES")

    # Add all 13 parameters
    params = [
        ("air", str(air)),
        ("tone", str(tone)),
        ("formant", str(formant)),
        ("resistance", str(resistance)),
        ("vibratoDepth", str(vibratoDepth)),
        ("vibratoRateHz", str(vibratoRateHz)),
        ("noiseColor", str(noiseColor)),
        ("sineAnchor", str(sineAnchor)),
        ("motionSustain", motionSustainStr),
        ("motionSensitivity", str(motionSensitivity)),
        ("attackMs", str(attackMs)),
        ("releaseMs", str(releaseMs)),
        ("outputGainDb", str(outputGainDb)),
    ]

    for param_name, param_value in params:
        param = ET.SubElement(values, "PARAM")
        param.set("id", param_name)
        param.set("value", param_value)

    # Pretty print XML
    xml_str = ET.tostring(root, encoding="unicode")
    dom = minidom.parseString(xml_str)
    pretty_xml = dom.toprettyxml(indent="  ")

    # Remove extra blank lines
    lines = [line for line in pretty_xml.split('\n') if line.strip()]
    return '\n'.join(lines)

def save_preset(filename, content):
    """Save preset to file"""
    os.makedirs(os.path.dirname(filename), exist_ok=True)
    with open(filename, 'w') as f:
        f.write(content)
    print(f"✅ Created: {filename}")

def main():
    print("🎵 Generating Breath Lead Presets (Motion Instrument)\n")

    # Preset definitions with all 13 parameters
    presets = [
        # === Init Patches ===
        {
            "name": "Default Init",
            "category": "Init",
            "params": {
                "air": 0.45,
                "tone": 0.50,
                "formant": 0.50,
                "resistance": 0.30,
                "vibratoDepth": 0.10,
                "vibratoRateHz": 5.0,
                "noiseColor": 0.70,
                "sineAnchor": 0.30,
                "motionSustain": True,
                "motionSensitivity": 0.60,
                "attackMs": 50.0,
                "releaseMs": 300.0,
                "outputGainDb": -3.0,
            },
            "description": "Default patch - sounds musical immediately"
        },

        # === Breath Intensity ===
        {
            "name": "Soft Breath",
            "category": "Breath",
            "params": {
                "air": 0.30,
                "tone": 0.50,
                "formant": 0.50,
                "resistance": 0.60,
                "vibratoDepth": 0.05,
                "vibratoRateHz": 5.0,
                "noiseColor": 0.60,
                "sineAnchor": 0.20,
                "motionSustain": True,
                "motionSensitivity": 0.50,
                "attackMs": 80.0,
                "releaseMs": 400.0,
                "outputGainDb": -6.0,
            },
            "description": "Gentle, intimate breath"
        },
        {
            "name": "Medium Breath",
            "category": "Breath",
            "params": {
                "air": 0.50,
                "tone": 0.60,
                "formant": 0.50,
                "resistance": 0.40,
                "vibratoDepth": 0.10,
                "vibratoRateHz": 5.0,
                "noiseColor": 0.70,
                "sineAnchor": 0.30,
                "motionSustain": True,
                "motionSensitivity": 0.60,
                "attackMs": 50.0,
                "releaseMs": 300.0,
                "outputGainDb": -3.0,
            },
            "description": "Balanced, versatile"
        },
        {
            "name": "Full Breath",
            "category": "Breath",
            "params": {
                "air": 0.80,
                "tone": 0.70,
                "formant": 0.50,
                "resistance": 0.30,
                "vibratoDepth": 0.15,
                "vibratoRateHz": 5.5,
                "noiseColor": 0.80,
                "sineAnchor": 0.40,
                "motionSustain": True,
                "motionSensitivity": 0.70,
                "attackMs": 30.0,
                "releaseMs": 200.0,
                "outputGainDb": 0.0,
            },
            "description": "Powerful, projecting"
        },

        # === Tone Colors ===
        {
            "name": "Dark",
            "category": "Tone",
            "params": {
                "air": 0.50,
                "tone": 0.20,
                "formant": 0.50,
                "resistance": 0.40,
                "vibratoDepth": 0.10,
                "vibratoRateHz": 5.0,
                "noiseColor": 0.50,
                "sineAnchor": 0.30,
                "motionSustain": True,
                "motionSensitivity": 0.60,
                "attackMs": 50.0,
                "releaseMs": 300.0,
                "outputGainDb": -3.0,
            },
            "description": "Warm, muted, intimate"
        },
        {
            "name": "Bright",
            "category": "Tone",
            "params": {
                "air": 0.50,
                "tone": 0.90,
                "formant": 0.50,
                "resistance": 0.40,
                "vibratoDepth": 0.10,
                "vibratoRateHz": 5.0,
                "noiseColor": 0.90,
                "sineAnchor": 0.30,
                "motionSustain": True,
                "motionSensitivity": 0.60,
                "attackMs": 50.0,
                "releaseMs": 300.0,
                "outputGainDb": -6.0,
            },
            "description": "Clear, present, cutting"
        },
        {
            "name": "Neutral",
            "category": "Tone",
            "params": {
                "air": 0.50,
                "tone": 0.50,
                "formant": 0.50,
                "resistance": 0.40,
                "vibratoDepth": 0.10,
                "vibratoRateHz": 5.0,
                "noiseColor": 0.70,
                "sineAnchor": 0.30,
                "motionSustain": True,
                "motionSensitivity": 0.60,
                "attackMs": 50.0,
                "releaseMs": 300.0,
                "outputGainDb": -3.0,
            },
            "description": "Balanced, natural"
        },

        # === Character ===
        {
            "name": "Airy",
            "category": "Character",
            "params": {
                "air": 0.70,
                "tone": 0.60,
                "formant": 0.30,
                "resistance": 0.20,
                "vibratoDepth": 0.10,
                "vibratoRateHz": 5.0,
                "noiseColor": 0.90,
                "sineAnchor": 0.20,
                "motionSustain": True,
                "motionSensitivity": 0.50,
                "attackMs": 40.0,
                "releaseMs": 500.0,
                "outputGainDb": -6.0,
            },
            "description": "Breath noise prominent, spacious"
        },
        {
            "name": "Focused",
            "category": "Character",
            "params": {
                "air": 0.50,
                "tone": 0.60,
                "formant": 0.70,
                "resistance": 0.80,
                "vibratoDepth": 0.05,
                "vibratoRateHz": 5.0,
                "noiseColor": 0.50,
                "sineAnchor": 0.50,
                "motionSustain": False,
                "motionSensitivity": 0.60,
                "attackMs": 20.0,
                "releaseMs": 100.0,
                "outputGainDb": 0.0,
            },
            "description": "Tight, controlled, centered"
        },
        {
            "name": "Open",
            "category": "Character",
            "params": {
                "air": 0.60,
                "tone": 0.70,
                "formant": 0.40,
                "resistance": 0.20,
                "vibratoDepth": 0.15,
                "vibratoRateHz": 5.5,
                "noiseColor": 0.70,
                "sineAnchor": 0.30,
                "motionSustain": True,
                "motionSensitivity": 0.70,
                "attackMs": 60.0,
                "releaseMs": 400.0,
                "outputGainDb": -3.0,
            },
            "description": "Loose, resonant, free"
        },

        # === Performance ===
        {
            "name": "Subtle Vibrato",
            "category": "Performance",
            "params": {
                "air": 0.50,
                "tone": 0.60,
                "formant": 0.50,
                "resistance": 0.40,
                "vibratoDepth": 0.15,
                "vibratoRateHz": 5.0,
                "noiseColor": 0.70,
                "sineAnchor": 0.30,
                "motionSustain": True,
                "motionSensitivity": 0.60,
                "attackMs": 50.0,
                "releaseMs": 300.0,
                "outputGainDb": -3.0,
            },
            "description": "Gentle oscillation, warmth"
        },
        {
            "name": "Wide Vibrato",
            "category": "Performance",
            "params": {
                "air": 0.50,
                "tone": 0.60,
                "formant": 0.50,
                "resistance": 0.40,
                "vibratoDepth": 0.50,
                "vibratoRateHz": 6.0,
                "noiseColor": 0.70,
                "sineAnchor": 0.30,
                "motionSustain": True,
                "motionSensitivity": 0.60,
                "attackMs": 50.0,
                "releaseMs": 300.0,
                "outputGainDb": -3.0,
            },
            "description": "Expressive wobble, classic"
        },
        {
            "name": "No Vibrato",
            "category": "Performance",
            "params": {
                "air": 0.50,
                "tone": 0.60,
                "formant": 0.50,
                "resistance": 0.40,
                "vibratoDepth": 0.0,
                "vibratoRateHz": 5.0,
                "noiseColor": 0.70,
                "sineAnchor": 0.30,
                "motionSustain": True,
                "motionSensitivity": 0.60,
                "attackMs": 50.0,
                "releaseMs": 300.0,
                "outputGainDb": -3.0,
            },
            "description": "Pure tone, steady"
        },

        # === Musical Styles ===
        {
            "name": "Flute-like",
            "category": "Style",
            "params": {
                "air": 0.60,
                "tone": 0.70,
                "formant": 0.60,
                "resistance": 0.50,
                "vibratoDepth": 0.20,
                "vibratoRateHz": 5.5,
                "noiseColor": 0.70,
                "sineAnchor": 0.30,
                "motionSustain": True,
                "motionSensitivity": 0.55,
                "attackMs": 60.0,
                "releaseMs": 200.0,
                "outputGainDb": -3.0,
            },
            "description": "Woodwind character, airy"
        },
        {
            "name": "Brassy",
            "category": "Style",
            "params": {
                "air": 0.70,
                "tone": 0.80,
                "formant": 0.50,
                "resistance": 0.60,
                "vibratoDepth": 0.25,
                "vibratoRateHz": 5.0,
                "noiseColor": 0.70,
                "sineAnchor": 0.40,
                "motionSustain": True,
                "motionSensitivity": 0.65,
                "attackMs": 30.0,
                "releaseMs": 150.0,
                "outputGainDb": 0.0,
            },
            "description": "Trumpet-like, bright"
        },
        {
            "name": "Vocal Ooh",
            "category": "Style",
            "params": {
                "air": 0.50,
                "tone": 0.50,
                "formant": 0.40,
                "resistance": 0.50,
                "vibratoDepth": 0.30,
                "vibratoRateHz": 5.5,
                "noiseColor": 0.60,
                "sineAnchor": 0.30,
                "motionSustain": True,
                "motionSensitivity": 0.60,
                "attackMs": 50.0,
                "releaseMs": 300.0,
                "outputGainDb": -3.0,
            },
            "description": "Vowel Ooh, warm"
        },
        {
            "name": "Vocal Aah",
            "category": "Style",
            "params": {
                "air": 0.50,
                "tone": 0.60,
                "formant": 0.60,
                "resistance": 0.40,
                "vibratoDepth": 0.30,
                "vibratoRateHz": 5.5,
                "noiseColor": 0.70,
                "sineAnchor": 0.30,
                "motionSustain": True,
                "motionSensitivity": 0.60,
                "attackMs": 50.0,
                "releaseMs": 300.0,
                "outputGainDb": -3.0,
            },
            "description": "Vowel Aah, open"
        },
        {
            "name": "Synth Lead",
            "category": "Style",
            "params": {
                "air": 0.40,
                "tone": 0.80,
                "formant": 0.50,
                "resistance": 0.30,
                "vibratoDepth": 0.40,
                "vibratoRateHz": 6.0,
                "noiseColor": 0.70,
                "sineAnchor": 0.40,
                "motionSustain": False,
                "motionSensitivity": 0.60,
                "attackMs": 20.0,
                "releaseMs": 100.0,
                "outputGainDb": 0.0,
            },
            "description": "Electronic, controlled"
        },

        # === Special Effects ===
        {
            "name": "Whisper",
            "category": "FX",
            "params": {
                "air": 0.90,
                "tone": 0.30,
                "formant": 0.30,
                "resistance": 0.10,
                "vibratoDepth": 0.0,
                "vibratoRateHz": 5.0,
                "noiseColor": 0.90,
                "sineAnchor": 0.10,
                "motionSustain": False,
                "motionSensitivity": 0.40,
                "attackMs": 100.0,
                "releaseMs": 600.0,
                "outputGainDb": -12.0,
            },
            "description": "Noisy, intimate"
        },
        {
            "name": "Punchy",
            "category": "FX",
            "params": {
                "air": 0.80,
                "tone": 0.70,
                "formant": 0.60,
                "resistance": 0.70,
                "vibratoDepth": 0.0,
                "vibratoRateHz": 5.0,
                "noiseColor": 0.70,
                "sineAnchor": 0.40,
                "motionSustain": False,
                "motionSensitivity": 0.60,
                "attackMs": 5.0,
                "releaseMs": 50.0,
                "outputGainDb": 0.0,
            },
            "description": "Fast attack, percussive"
        },
        {
            "name": "Evanescent",
            "category": "FX",
            "params": {
                "air": 0.30,
                "tone": 0.80,
                "formant": 0.40,
                "resistance": 0.20,
                "vibratoDepth": 0.20,
                "vibratoRateHz": 7.0,
                "noiseColor": 0.80,
                "sineAnchor": 0.30,
                "motionSustain": True,
                "motionSensitivity": 0.70,
                "attackMs": 150.0,
                "releaseMs": 800.0,
                "outputGainDb": -9.0,
            },
            "description": "Delicate, fading"
        },

        # === Motion Sustain Showcase ===
        {
            "name": "Motion Sustain Demo",
            "category": "Motion",
            "params": {
                "air": 0.50,
                "tone": 0.60,
                "formant": 0.50,
                "resistance": 0.30,
                "vibratoDepth": 0.10,
                "vibratoRateHz": 5.0,
                "noiseColor": 0.70,
                "sineAnchor": 0.30,
                "motionSustain": True,
                "motionSensitivity": 0.90,
                "attackMs": 50.0,
                "releaseMs": 50.0,
                "outputGainDb": -3.0,
            },
            "description": "Demonstrates motion sustain - wiggle controls to sustain notes!"
        },
    ]

    # Generate presets
    output_dir = "presets"
    categories = {}

    for preset in presets:
        category = preset["category"]
        name = preset["name"]
        description = preset["description"]
        params = preset["params"]

        # Create category directory
        category_dir = os.path.join(output_dir, category)
        os.makedirs(category_dir, exist_ok=True)

        # Generate preset XML
        xml_content = create_preset(
            name=name,
            air=params["air"],
            tone=params["tone"],
            formant=params["formant"],
            resistance=params["resistance"],
            vibratoDepth=params["vibratoDepth"],
            vibratoRateHz=params["vibratoRateHz"],
            noiseColor=params["noiseColor"],
            sineAnchor=params["sineAnchor"],
            motionSustain=params["motionSustain"],
            motionSensitivity=params["motionSensitivity"],
            attackMs=params["attackMs"],
            releaseMs=params["releaseMs"],
            outputGainDb=params["outputGainDb"],
        )

        # Save preset
        filename = os.path.join(category_dir, f"{name}.xml")
        save_preset(filename, xml_content)

        # Track by category
        if category not in categories:
            categories[category] = []
        categories[category].append(name)

    # Generate index
    print("\n" + "="*60)
    print("📚 Preset Library Index")
    print("="*60 + "\n")

    for category in sorted(categories.keys()):
        print(f"\n{category}")
        print("-" * len(category))
        for preset_name in categories[category]:
            print(f"  • {preset_name}")

    print(f"\n{'='*60}")
    print(f"✅ Generated {len(presets)} presets across {len(categories)} categories")
    print(f"{'='*60}\n")

    print("📁 Output directory: presets/")
    print("\n📖 Installation instructions:")
    print("   macOS: Copy to ~/Library/Audio/Presets/SchillingerEcosystem/BreathLead/")
    print("   Windows: Copy to C:\\Users\\<User>\\AppData\\Roaming\\SchillingerEcosystem\\BreathLead\\Presets\\")
    print()

if __name__ == "__main__":
    main()
