# SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
#
# SPDX-License-Identifier: MIT

"""Write llms.txt, an index of the docs for LLM agents (https://llmstxt.org/).

It reuses what the HTML build already parsed: one line per page with its title
and first paragraph, in table-of-contents order. Read the Docs serves each page
as Markdown on request, so the index links the normal HTML pages.
"""

from docutils import nodes
from sphinx import addnodes

DESCRIPTION_LENGTH = 160


def page_order(env, docname, seen):
    """Yield docnames depth-first in table-of-contents order."""
    if docname in seen:
        return
    seen.add(docname)
    yield docname
    for child in env.toctree_includes.get(docname, []):
        yield from page_order(env, child, seen)


def is_skipped(node):
    """Raw HTML, parameter lists and notes; an API description counts."""
    if isinstance(node, (nodes.raw, nodes.field_list, nodes.comment)):
        return True
    # Sphinx derives API descriptions from Admonition too, so keep those.
    return isinstance(node, nodes.Admonition) and not isinstance(node, addnodes.desc)


def first_paragraph(doctree):
    """Plain text of the first paragraph worth using as a description."""
    for paragraph in doctree.findall(nodes.paragraph):
        parent = paragraph.parent
        while parent is not None and not is_skipped(parent):
            parent = parent.parent
        if parent is not None:
            continue
        text = " ".join(paragraph.astext().split())
        if text:
            if len(text) > DESCRIPTION_LENGTH:
                text = text[: DESCRIPTION_LENGTH - 3].rstrip() + "..."
            return text
    return ""


def write_llms_txt(app, exception):
    if exception is not None or app.builder.name not in ("html", "dirhtml"):
        return
    env = app.env
    excluded = set(app.config.llms_txt_exclude)
    lines = ["# " + app.config.project, ""]
    if app.config.llms_txt_description:
        lines += ["> " + app.config.llms_txt_description, ""]
    lines += ["## Pages", ""]
    seen = set()
    ordered = list(page_order(env, app.config.root_doc, seen))
    ordered += sorted(env.found_docs - seen)
    for docname in ordered:
        if docname in excluded or docname not in env.titles:
            continue
        title = env.titles[docname].astext().strip()
        description = first_paragraph(env.get_doctree(docname))
        link = app.builder.get_target_uri(docname)
        entry = "- [{}]({})".format(title, link)
        if description:
            entry += ": " + description
        lines.append(entry)
    with open(app.outdir / "llms.txt", "w", encoding="utf-8") as f:
        f.write("\n".join(lines) + "\n")


def setup(app):
    app.add_config_value("llms_txt_description", "", "html")
    app.add_config_value("llms_txt_exclude", [], "html")
    app.connect("build-finished", write_llms_txt)
    return {"version": "1.0", "parallel_read_safe": True, "parallel_write_safe": True}
