import re

with open("launcher/ui/widgets/HalkyNavBar.cpp", "r", encoding="utf-8") as f:
    content = f.read()

if "#include <QAbstractAnimation>" not in content:
    content = content.replace("#include <QVariantAnimation>", "#include <QVariantAnimation>\n#include <QAbstractAnimation>")
    with open("launcher/ui/widgets/HalkyNavBar.cpp", "w", encoding="utf-8") as f:
        f.write(content)
