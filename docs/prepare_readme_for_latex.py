import shutil

with open("README.rst", "r") as f:
    readme_content = f.read()

shutil.copyfile("README.rst", "README.rst.bak")

# turn badge into text only
modified_readme_content = readme_content.replace("|Weblate|", "Weblate", 1)

# remove image link
badge_link_lines = """.. |Weblate| image:: https://hosted.weblate.org/widgets/circuitpython/-/svg-badge.svg
   :target: https://hosted.weblate.org/engage/circuitpython/?utm_source=widget"""

modified_readme_content = modified_readme_content.replace(badge_link_lines, "")

with open("README.rst", "w") as f:
    f.write(modified_readme_content)
