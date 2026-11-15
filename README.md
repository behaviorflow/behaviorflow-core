# Behavior Flow

## Overview

Behavior Flow is a prospective library for modeling and executing high-level robot behavior and decision-making.

## How it works

Discrete behaviors (e.g. move, sense, etc.) are defined by a user in C++ and then abstracted as **nodes** that can be integrated into the Behavior Flow model. In this model, nodes are arranged **parallelly or sequentially**. The execution order of nodes is determined at runtime based on the outcome of previous nodes, or on any defined conditions.

For execution logic that is better defined by code, users can define **control wrapper** nodes, which wrap nodes or sequences of nodes and execute them in whatever way is defined. Control nodes fit into the flow like any other node.

Finally, users can encapsulate node flows into **sub-flows**, which can be reused in multiple locations and make this tool easier to work with.

These models are stored as human-readable formatted text. However, we recommend using our Behavior Flow Editor GUI to define the models, then exporting the text representation from there.

![bf_transfer](https://github.com/user-attachments/assets/4bf18a83-e96d-49bb-8512-c0250ed47cec)

## Roadmap

Coming soon...
