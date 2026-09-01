---
layout: default
order: 0
title: "🤔 How to Evidence"
preview_title: "🤔"
---

<!-- markdownlint-disable-file MD033 -->

## Legend

Before you delve into your Learning Log, we would like to emphasize a couple of instructions on how to evidence your work in Y3.

1. You **MUST** tag all your evidence.
2. Please use the provided color emoticons when you specify a tag, this will make it easier for us to spot them and search for them.\
🔴 **Poor**\
🟠 **Insufficient**\
🟡 **Sufficient**\
🟢 **Good**\
🔵 **Excellent**
3. Tags should follow this format **\<color\> [ILO \<number of the ILO\> \<level of the ILO\>]**. For example:\
    🟢 **[ILO 3 Good]**\
    🔵 **[ILO 4 Excellent]**\
    🟡 **[ILO 1 Sufficient]**
4. Where applicable, reference tasks from your task overview ( "Summary of Project Work" and "Summary of Self-Development Work") in your evidence. Tasks should follow this format:\
**🟣 [Task: \<name of task\>]**\
For example, if you have a task in your "Summary of Project Work" with the name "Grappling Hook" you should reference it as follows:\
🟣 [Task: Grappling Hook]\
you could also link it to it using the demonstrated code:

```md
🟣[Custom foo description](#foo)
# Foo
```

---

## Example of tagging

These are some examples of how you could tag your work. When you **reference other evidence** within a certain ILO please **provide a hyperlink** to it. Details are overlooked quite fast when digging through multiple learning logs within a day, hyperlinks are highlighted within text, this makes them jump out more therefore references are missed less often.

<a name="poor"></a>
🔴 **[ILO 2 Poor]**

| Task Name | Task Description | Time Estimated | Time Tracked |
| --------- | ---------------- | -------------- | ------------ |
| Task X    | ...              | x h            | x h          |
| Task Y    | ...              | x h            | x h          |
| ...       | ...              | x h            | x h          |

<a name="insufficient"></a>
🟠 **[ILO 2 Insufficient]**

**Total Hours:** x h

<a name="poor"></a>
🔴 **[ILO 3 Poor]**

Here you can find the results of the first peer review: [Link To Peer Review](https://www.google.com)
Here you can find the results of the second peer review: [Link To Peer Review](https://www.google.com)

Confirmation of completing the Roundtable Survey:

![Roundtable Survey](/assets/media/example_tag_image.png)

Here you can find information on how to reference research and resources ( second part of the resource referencing is at the bottom of this page )

[Resource Link 00][^1]\
[Resource Link 01][^2]\
[Resource Link 02][^3]

<a name="good"></a>
🟢 **[ILO 3 Good]**

I did three things that are relevant for this ILO.

The first was the work I did in week 4 related to...

<a name="insufficient"></a>
🟠 **[ILO 4 Insufficient]**

While working on the Physics API, I followed these specific pipelines and process ...

---

[^1]: [Google](https://www.google.com)
[^2]: [Google](https://www.google.com)
[^3]: [Google](https://www.google.com)

## Retakes

This Learning Log template supports multiple opportunities (versions) for the course. Here's how it works:

### First Opportunity (`_v1` folder)

For your **first opportunity**, work on the ILO files in the `_v1` folder. This folder contains all the ILO markdown files (ILO1.md, ILO2.md, etc.) where you should document your evidence throughout the block.

### Retake Opportunity (`_v2` folder)

If you need to do a **retake** (second opportunity):

1. **Copy your ILO files** from the `_v1` folder to the `_v2` folder
2. **Add the additional evidence to the files in `_v2`** with your new evidence for the retake opportunity. Make sure it is clear what has been added for the retake.
3. Keep both versions - teachers will be able to switch between v1 and v2 to see your progress

### Third Opportunity (`_v3` folder)

If you need a **third opportunity**:

1. **Copy the entire `_v2` folder** and rename the copy to `_v3`
2. **Update [_config.yml](_config.yml)** to add the v3 collection:

   ```yaml
   collections:
     v1:
       output: true
     v2:
       output: true
     v3:
       output: true
   ```

3. **Add the additional evidence to the files in `_v3`** with your new evidence for the third opportunity. Make sure it is clear what has been added for the 3rd opportunity.
4. The version dropdown will automatically include v3 once you rebuild the site

### Version Dropdown on the Website

When your Learning Log website is generated, teachers will see a version dropdown in the top-right corner of each ILO page. This allows them to:

* View your first opportunity evidence (v1)
* View your retake opportunity evidence (v2, v3, etc.)
* Easily compare between versions while staying on the same ILO

**Note:** For most students, you will only need to work in the `_v1` folder. Only use `_v2` or `_v3` if you're doing a retake.

**Evidence for the retake should be filled out underneath the original evidence** clearly tagged using the same rules described above with the additional **retake** keyword, an example can be seen below.

<a name="good"></a>
🟢 **[ILO 3 Good - Retake]**

I did three things that are relevant for this ILO.

The first was the work I did in week 4 related to...
