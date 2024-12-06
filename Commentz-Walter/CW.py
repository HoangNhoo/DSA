"""
File: structures.py
-------------------
Final Project: Commentz-Walter String Matching Algorithm
Course: CS 166
Authors: Eric Ehizokhale and Jake Rachleff

Implements three data structures: Trie, Aho Corsick, and Commentz-Walter
"""

from collections import deque


NOT_SET = -1

class Node(object):
	def __init__(self, character, depth, parent):
		self.character = character  # character of the node
		self.depth = depth          # depth of the node
		self.word = None            # word that the node represents
		self.parent = parent        # parent of the node
		self.ACsuffix_link = None   # Aho-Corsick suffix link
		self.ACoutput_link = None   # Aho-Corsick output link
		self.children = {}          # children of the node


class CWNode(Node):
	def __init__(self, character, depth, parent):
		Node.__init__(self, character, depth, parent)
		self.min_difference_s1 = NOT_SET        # minimum difference between depth of node and depth of suffix link
		self.min_difference_s2 = NOT_SET        # minimum difference between depth of node and depth of output link
		self.CWsuffix_link = None               # Commentz-Walter suffix link, datatype: CWNode
		self.CWoutput_link = None               # Commentz-Walter output link, datatype: CWNode
		self.shift1 = None                      # shift value 1
		self.shift2 = None                      # shift value 2

class Trie(object):
	def create_node(self, character, depth, parent):
		return Node(character, depth, parent)

	def __init__(self):
		self.size = 0
		self.root = self.create_node(None, 0, None)	#char doesn't exist for root

	def add_word(self, word):
		current_node = self.root
		current_depth = 1
		for character in word:
			next_node = current_node.children.get(character)
			if not next_node:
				next_node = self.create_node(character, current_depth, current_node)
				current_node.children[character] = next_node

			current_node = next_node
			current_depth += 1

		if current_node.word is not None:
			#print ("you have already printed " + word)
			return

		current_node.word = word
		self.size += 1

	def lookup(self,word):
		current_node = self.root
		for character in word:
			next_node = current_node.children.get(character)
			if not next_node:
				return False

			current_node = next_node

		return True

	def is_root(self, node):
		return node.character is None

	def node_has_child(self, node, character):
		return node.children.get(character) is not None

	def get_AC_suffix_link(self, node):
		searcher = node.parent.ACsuffix_link        # start at parent's suffix link
		while (not self.is_root(searcher)) and (not self.node_has_child(searcher, node.character)):
			searcher = searcher.ACsuffix_link
			if not searcher:
				import pdb; pdb.set_trace()

		if (self.node_has_child(searcher, node.character)):
			return searcher.children[node.character]
		else:
			if (not self.is_root(searcher)):
				print ("ERROR: Incorrect looping in suffix links")
			return searcher



class CWAuto(Trie):
	def create_node(self, character, depth, parent):
		return CWNode(character, depth, parent)

	def __init__(self):
		Trie.__init__(self)
		self.min_depth = None
		self.char_lookup_table = {}     # dictionary of characters and their minimum depths

	def add_word(self, word):
		word = word[::-1]
		super().add_word(word)
		pos = 1

		#Initialize character table
		#If character is not in table, set it to the current position
		#If character is in table, update it to the current position
		for character in word:
			min_char_depth = self.char_lookup_table.get(character)      # get the minimum depth of the character
			if (min_char_depth is None) or (min_char_depth > pos):      # if the character is not in the table or the current position is less than the current depth
				# cba, cb
				# abc, bc
				# [a] = 1, [b] = 2, [c] = 3
				# [b] = 1, [c] = 2
				self.char_lookup_table[character] = pos
			pos += 1

		if self.min_depth is None:
			self.min_depth = len(word)
		elif len(word) < self.min_depth:
			self.min_depth = len(word)

	def lookup(self, word):
		word = word[::-1]
		super().lookup(word)

	def initialize_shift_values(self):
		bfs_queue = deque()
		self.root.shift1 = 1
		self.root.shift2 = self.min_depth

		for key in self.root.children:
			bfs_queue.append(self.root.children[key])

		while (len(bfs_queue) > 0):
			current_node = bfs_queue.popleft()
			# set shift1
			if current_node.CWsuffix_link is None:
				current_node.shift1 = self.min_depth
			else:
				current_node.shift1 = current_node.min_difference_s1

			#set shift2
			if current_node.CWoutput_link is None:
				current_node.shift2 = current_node.parent.shift2
			else:
				current_node.shift2 = current_node.min_difference_s2

			for key in current_node.children:
				bfs_queue.append(current_node.children[key])

	def create_failure_links(self):
		bfs_queue = deque()

		# First, set suffix links for first children to root
		for key in self.root.children:
			child = self.root.children[key]
			child.ACsuffix_link = self.root

			for key2 in child.children:
				grandchild = child.children[key2]
				bfs_queue.append(grandchild)

		while (len(bfs_queue) > 0):
			current_node = bfs_queue.popleft()
			for key in current_node.children:
				child = current_node.children[key]
				bfs_queue.append(child)

			# Set AC nodes first
			AC_suffix_node = self.get_AC_suffix_link(current_node)
			current_node.ACsuffix_link = AC_suffix_node
			suffix_is_word = current_node.ACsuffix_link.word is not None
			current_node.ACoutput_link = current_node.ACsuffix_link if suffix_is_word else current_node.ACsuffix_link.ACoutput_link
			if current_node.ACoutput_link is not None:
				pass

			# Set reverse suffix links and output links
			is_set2 = current_node.word is not None
			if AC_suffix_node.min_difference_s1 == -1 or AC_suffix_node.min_difference_s1 > current_node.depth - AC_suffix_node.depth:
				AC_suffix_node.min_difference_s1 = current_node.depth - AC_suffix_node.depth
				AC_suffix_node.CWsuffix_link = current_node
			if is_set2:
				if AC_suffix_node.min_difference_s2 == -1 or AC_suffix_node.min_difference_s2 > current_node.depth - AC_suffix_node.depth:
					AC_suffix_node.min_difference_s2 = current_node.depth - AC_suffix_node.depth
					AC_suffix_node.CWoutput_link = current_node

		self.initialize_shift_values()

	def char_func(self, character):
		min_depth = self.char_lookup_table.get(character)
		if min_depth is None:
			min_depth = self.min_depth + 1

		return min_depth

	def shift_func(self, node, j):
		max_of_s1_and_char = 0                  # max of shift1 and char_func
		if node.character is None:
			max_of_s1_and_char = node.shift1    # root node
		else:
			max_of_s1_and_char = max(self.char_func(node.character) - j - 1, node.shift1)   
		return min(max_of_s1_and_char, node.shift2) 

	def report_all_matches(self, text):
		i = self.min_depth - 1
		matches = deque()

		while (i < len(text)):
			# Scan Phase
			v = self.root
			j = 0
			char_to_find = text[i - j]
			while self.node_has_child(v, char_to_find):
				v = v.children[char_to_find]
				j += 1

				if (v.word is not None):
					matches.append((v.word[::-1], i - j + 1))

				if (i - j >= 0):
					char_to_find = text[i - j]
				else:
					break
					
			if (j > i):
				j = i

			i += self.shift_func(v, j)

		return len(matches)

