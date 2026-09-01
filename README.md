---
layout: default
title: README
---

## 2627-Y3A-PR-Template Repository

Hello, BUas Games student and aspiring game developer! This template repository will be the basis for your work in Year 3, Programming. The repository will contain your Learning Log.

## What is in this template?

* This template is used for your Learning Log evidence. We've prepared a set of Markdown (`.md`) files containing the Learning Log template for you to fill in.
* The template does not contain any code; each team will receive a repository which is used to collaborate on code.

## What to hand in, and how?

* You need to ***create a release of your repo through GitHub***. This will generate a nicely formatted GitHub Pages website for the Learning Log.
* Download a ZIP file of your GitHub repo and add this to your assignment in Brightspace.
* Please ***submit a link to the generated website of your repository*** to your assignment in Brightspace.
* Please make sure that you **verify that the link works and the content is displayed correctly and videos play**!

## Introduction to the Learning Log

For this block, **we've put the Learning Log entirely inside GitHub**. The Markdown files in this folder replace the PowerPoint that you've used in previous years.
Because the LL is now part of your codebase, you can work on it straight from your IDE, and you can easily embed your code into your evidence.
We hope this will make evidencing more intuitive for programmers.

> [!IMPORTANT]
>
> * **Keep your Learning Log up-to-date throughout the block.** We recommend to sync it with your work at least once per week.
> * The Learning Log will be the basis for your 1-on-1s. Use it to prepare for your feedback sessions, and make sure it contains useful conversation points.
> * The Learning Log will also be the teachers' main resource for grading at the end of the block. Make sure your evidence speaks for itself, without a need to dive into your codebase unguided.

## Add your personal info

First of all, make sure that the template contains enough of your personal details:

1. In [_config.yml](_config.yml), fill in your student name, number, and team or project name. This will display in the header of each page of the generated website.
2. In [_config.yml](_config.yml), add a link to your profile picture. This can be a path to an image file in the repo or a URL to your profile picture (for example, a Gravatar link). This picture will be used in [index.md](index.md) to show a picture of you.

## Sections

The index page should contain your personal info, as well as your end-of-block reflection and self-assessment.

Next to this, there is a separate Markdown file per ILO. Each ILO file starts with set of guidelines and suggestions. Please adjust these to your needs.

ILO files have no prescribed structure and can be filled in however you want but **don't forget to tag your evidence**. For more instructions on how to evidence, see the [how-to-evidence](how-to-evidence)

## Version System (First Opportunity & Retakes)

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

### Markdown cheatsheet

For a quick overview of how to use Markdown, have a look at the small [cheatsheet](cheatsheet.md) we've prepared!

### How to evidence

The how to evidence page contains all the information on how you should evidence your learning log.

## What should you include as evidence?

Evidence is not just about the end result; it's the journey. Here's what you should consider incorporating:

* **Multimedia**: Add videos, screenshots, diagrams wherever they add value. Visual representation can often convey more than words. Warning: videos can become large files very quickly, so use them wisely (not too long, reasonable resolution, proper compression, etc).
* **Link to Code**: You can provide links to specific lines of your code.
* **Code Snippets**: Uou can easily provide code snippets using Markdown.
* **GitHub Issues**: If you choose to use GitHub issues to plan out your tasks, you code create screenshots of them and embed them here as images.
* **External Documentation / AI Tools**: If your work is based on external articles, papers, codebases, or other resources, refer to them with the appropriate names and URLs. The same goes for any tools that you have used, including AI tools like ChatGPT.
* **Feedback Summaries**: Feedback, whether from peers or instructors, is a valuable part of learning. Summarize and reflect on the feedback received to show your responsive actions and learning.

## Website instructions

This GitHub repo has been set up so that it can easily generate a nice-looking website out of your Learning Log Markdown files. The easiest way to trigger this is to **push to the main branch**. You should do this at least once a week and have your published learning log ready before your feedback session!

Before submitting your assignment, make sure to test if all your videos etc show up correctly on the published website!

## Running the Learning Log website locally

We recommend to also generate the Learning Log website locally on a regular basis.
On Windows, this can be done as follows:

1. Install Ruby
    1. Download and install **Ruby+Devkit** from [RubyInstaller for Windows](https://rubyinstaller.org/). You'll see versions like `Ruby+Devkit 3.4.X (x64)`. Choose a version based on your Windows (64-bit or 32-bit). Most modern systems will use 64-bit.

    2. During the installation:
        * Use the default options.
        * Ensure the "Add Ruby executables to your PATH" option is checked.
        * Once the installation is finished, you'll be prompted to install MSYS2 development toolchain. Go ahead and let it install as it's required for Jekyll and some Ruby gems.

2. Navigate to your repository root folder and open the terminal/command prompt
3. Install Bundler and Jekyll

```bash
gem install jekyll bundler
```

1. Install Dependencies:

```bash
bundle install
```

1. Build and Run the Jekyll Site

```bash
bundle exec jekyll serve
```

Now, you should be able to access the site locally at [http://localhost:4000](http://localhost:4000).
