/*
 * Copyright 1999-2004 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#if !defined(XALANNODE_HEADER_GUARD_1357924680)
#define XALANNODE_HEADER_GUARD_1357924680



#include <xalanc/XalanDOM/XalanDOMDefinitions.hpp>



XALAN_CPP_NAMESPACE_BEGIN



class XalanDOMString;
class XalanDocument;
class XalanNamedNodeMap;
class XalanNodeList;



/*
 * <meta name="usage" content="experimental"/>
 *
 * Base class for the DOM Node interface.
 *
 * This class is experimental and subject to change!!
 */

class XALAN_DOM_EXPORT XalanNode
{
public:

	XalanNode();

	virtual
	~XalanNode();

	enum NodeType
	{
		UNKNOWN_NODE				= 0,
		ELEMENT_NODE				= 1,
		ATTRIBUTE_NODE				= 2,
		TEXT_NODE					= 3,
		CDATA_SECTION_NODE			= 4,
		ENTITY_REFERENCE_NODE		= 5,
		ENTITY_NODE 				= 6,
		PROCESSING_INSTRUCTION_NODE = 7,
		COMMENT_NODE				= 8,
		DOCUMENT_NODE				= 9,
		DOCUMENT_TYPE_NODE			= 10,
		DOCUMENT_FRAGMENT_NODE		= 11,
		NOTATION_NODE				= 12
	};

	typedef unsigned long	IndexType;

	virtual const XalanDOMString&
	getNodeName() const = 0;

	/**
	 * Gets the value of this node, depending on its type.
	 */
	virtual const XalanDOMString&
	getNodeValue() const = 0;

	/**
	 * An enum value representing the type of the underlying object.
	 */
	virtual NodeType
	getNodeType() const = 0;

	/**
	 * Gets the parent of this node.
	 *
	 * All nodes, except <code>Document</code>,
	 * <code>DocumentFragment</code>, and <code>Attr</code> may have a parent.
	 * However, if a node has just been created and not yet added to the tree,
	 * or if it has been removed from the tree, a <code>null</code> Node
	 * is returned.
	 */
	virtual XalanNode*
	getParentNode() const = 0;

	/**
	 * Gets a <code>NodeList</code> that contains all children of this node.
	 *
	 * If there
	 * are no children, this is a <code>NodeList</code> containing no nodes.
	 * The content of the returned <code>NodeList</code> is "live" in the sense
	 * that, for instance, changes to the children of the node object that
	 * it was created from are immediately reflected in the nodes returned by
	 * the <code>NodeList</code> accessors; it is not a static snapshot of the
	 * content of the node. This is true for every <code>NodeList</code>,
	 * including the ones returned by the <code>getElementsByTagName</code>
	 * method.
	 */
	virtual const XalanNodeList*
	getChildNodes() const = 0;

	/**
	 * Gets the first child of this node.
	 *
	 * If there is no such node, this returns <code>null</code>.
	 */
	virtual XalanNode*
	getFirstChild() const = 0;

	/**
	 * Gets the last child of this node.
	 *
	 * If there is no such node, this returns <code>null</code>.
	 */
	virtual XalanNode*
	getLastChild() const = 0;

	/**
	 * Gets the node immediately preceding this node.
	 *
	 * If there is no such node, this returns <code>null</code>.
	 */
	virtual XalanNode*
	getPreviousSibling() const = 0;

	/**
	 * Gets the node immediately following this node.
	 *
	 * If there is no such node, this returns <code>null</code>.
	 */
	virtual XalanNode*
	getNextSibling() const = 0;

	/**
	 * Gets a <code>NamedNodeMap</code> containing the attributes of this node (if it
	 * is an <code>Element</code>) or <code>null</code> otherwise.
	 */
	virtual const XalanNamedNodeMap*
	getAttributes() const = 0;

	/**
	 * Gets the <code>Document</code> object associated with this node.
	 *
	 * This is also
	 * the <code>Document</code> object used to create new nodes. When this
	 * node is a <code>Document</code> or a <code>DocumentType</code>
	 * which is not used with any <code>Document</code> yet, this is
	 * <code>null</code>.
	 */
	virtual XalanDocument*
	getOwnerDocument() const = 0;

	//@}
	/** @name Cloning function. */
	//@{

	/**
	 * Returns a duplicate of this node.
	 *
	 * This function serves as a generic copy constructor for nodes.
	 *
	 * The duplicate node has no parent (
	 * <code>parentNode</code> returns <code>null</code>.).
	 * <br>Cloning an <code>Element</code> copies all attributes and their
	 * values, including those generated by the  XML processor to represent
	 * defaulted attributes, but this method does not copy any text it contains
	 * unless it is a deep clone, since the text is contained in a child
	 * <code>Text</code> node. Cloning any other type of node simply returns a
	 * copy of this node.
	 * @param deep If <code>true</code>, recursively clone the subtree under the
	 *	 specified node; if <code>false</code>, clone only the node itself (and
	 *	 its attributes, if it is an <code>Element</code>).
	 * @return The duplicate node.
	 */
	virtual XalanNode*
	cloneNode(bool deep) const = 0;

	//@}
	/** @name Functions to modify the DOM Node. */
	//@{

	/**
	 * Inserts the node <code>newChild</code> before the existing child node
	 * <code>refChild</code>.
	 *
	 * If <code>refChild</code> is <code>null</code>,
	 * insert <code>newChild</code> at the end of the list of children.
	 * <br>If <code>newChild</code> is a <code>DocumentFragment</code> object,
	 * all of its children are inserted, in the same order, before
	 * <code>refChild</code>. If the <code>newChild</code> is already in the
	 * tree, it is first removed.  Note that a <code>Node</code> that
	 * has never been assigned to refer to an actual node is == null.
	 * @param newChild The node to insert.
	 * @param refChild The reference node, i.e., the node before which the new
	 *	 node must be inserted.
	 * @return The node being inserted.
	 */
	virtual XalanNode*
	insertBefore(
			XalanNode*	newChild,
			XalanNode*	refChild) = 0;

	/**
	 * Replaces the child node <code>oldChild</code> with <code>newChild</code>
	 * in the list of children, and returns the <code>oldChild</code> node.
	 *
	 * If <CODE>newChild</CODE> is a <CODE>DocumentFragment</CODE> object,
	 * <CODE>oldChild</CODE> is replaced by all of the <CODE>DocumentFragment</CODE>
	 * children, which are inserted in the same order.
	 *
	 * If the <code>newChild</code> is already in the tree, it is first removed.
	 * @param newChild The new node to put in the child list.
	 * @param oldChild The node being replaced in the list.
	 * @return The node replaced.
	 */
	virtual XalanNode*
	replaceChild(
			XalanNode*	newChild,
			XalanNode*	oldChild) = 0;

	/**
	 * Removes the child node indicated by <code>oldChild</code> from the list
	 * of children, and returns it.
	 *
	 * @param oldChild The node being removed.
	 * @return The node removed.
	 */
	virtual XalanNode*
	removeChild(XalanNode*	oldChild) = 0;

	/**
	 * Adds the node <code>newChild</code> to the end of the list of children of
	 * this node.
	 *
	 * If the <code>newChild</code> is already in the tree, it is
	 * first removed.
	 * @param newChild The node to add.If it is a  <code>DocumentFragment</code>
	 *	 object, the entire contents of the document fragment are moved into
	 *	 the child list of this node
	 * @return The node added.
	 */
	virtual XalanNode*
	appendChild(XalanNode*	newChild) = 0;

	//@}
	/** @name Query functions. */
	//@{

	/**
	 *	This is a convenience method to allow easy determination of whether a
	 * node has any children.
	 *
	 * @return	<code>true</code> if the node has any children,
	 *	 <code>false</code> if the node has no children.
	 */
	virtual bool
	hasChildNodes() const = 0;


	//@}
	/** @name Set functions. */
	//@{


	/**
	* Sets the value of the node.
	*
	* Any node which can have a nodeValue (@see getNodeValue) will
	* also accept requests to set it to a string. The exact response to
	* this varies from node to node -- Attribute, for example, stores
	* its values in its children and has to replace them with a new Text
	* holding the replacement value.
	*
	* For most types of Node, value is null and attempting to set it
	* will throw DOMException(NO_MODIFICATION_ALLOWED_ERR). This will
	* also be thrown if the node is read-only.
	*/
	virtual void
	setNodeValue(const XalanDOMString&		nodeValue) = 0;

	//@}
	/** @name Functions introduced in DOM Level 2. */
	//@{

	/**
	 * Puts all <CODE>Text</CODE>
	 * nodes in the full depth of the sub-tree underneath this <CODE>Node</CODE>, 
	 * including attribute nodes, into a "normal" form where only markup (e.g., 
	 * tags, comments, processing instructions, CDATA sections, and entity 
	 * references) separates <CODE>Text</CODE>
	 * nodes, i.e., there are no adjacent <CODE>Text</CODE>
	 * nodes. This can be used to ensure that the DOM view of a document is the 
	 * same as if it were saved and re-loaded, and is useful when operations 
	 * (such as XPointer lookups) that depend on a particular document tree 
	 * structure are to be used.
	 * <P><B>Note:</B> In cases where the document contains <CODE>CDATASections</CODE>, 
	 * the normalize operation alone may not be sufficient, since XPointers do 
	 * not differentiate between <CODE>Text</CODE>
	 * nodes and <CODE>CDATASection</CODE> nodes.</P>
	 */
	virtual void
	normalize() = 0;

	/**
	 * Tests whether the DOM implementation implements a specific
	 * feature and that feature is supported by this node.
	 * @param feature The string of the feature to test. This is the same
	 * name as what can be passed to the method <code>hasFeature</code> on
	 * <code>DOMImplementation</code>.
	 * @param version This is the version number of the feature to test. In
	 * Level 2, version 1, this is the string "2.0". If the version is not
	 * specified, supporting any version of the feature will cause the
	 * method to return <code>true</code>.
	 * @return Returns <code>true</code> if the specified feature is supported
	 * on this node, <code>false</code> otherwise.
	 */
	virtual bool
	isSupported(
			const XalanDOMString&	feature,
			const XalanDOMString&	version) const = 0;

	/**
	 * Get the <em>namespace URI</em> of
	 * this node, or <code>null</code> if it is unspecified.
	 * <p>
	 * This is not a computed value that is the result of a namespace lookup
	 * based on an examination of the namespace declarations in scope. It is
	 * merely the namespace URI given at creation time.
	 * <p>
	 * For nodes of any type other than <CODE>ELEMENT_NODE</CODE> and 
	 * <CODE>ATTRIBUTE_NODE</CODE> and nodes created with a DOM Level 1 method, 
	 * such as <CODE>createElement</CODE> from the <CODE>Document</CODE>
	 * interface, this is always <CODE>null</CODE>.
	 */
	virtual const XalanDOMString&
	getNamespaceURI() const = 0;

	/**
	 * Get the <em>namespace prefix</em>
	 * of this node, or <code>null</code> if it is unspecified.
	 */
	virtual const XalanDOMString&
	getPrefix() const = 0;

	/**
	 * Returns the local part of the <em>qualified name</em> of this node.
	 * <p>
	 * For nodes created with a DOM Level 1 method, such as
	 * <code>createElement</code> from the <code>Document</code> interface,
	 * it is null.
	 */
	virtual const XalanDOMString&
	getLocalName() const = 0;

	/**
	 * Set the <em>namespace prefix</em> of this node.
	 * <p>
	 * Note that setting this attribute, when permitted, changes 
	 * the <CODE>nodeName</CODE> attribute, which holds the <EM>qualified 
	 * name</EM>, as well as the <CODE>tagName</CODE> and <CODE>name</CODE> 
	 * attributes of the <CODE>Element</CODE> and <CODE>Attr</CODE>
	 * interfaces, when applicable.
	 * <p>
	 * Note also that changing the prefix of an 
	 * attribute, that is known to have a default value, does not make a new 
	 * attribute with the default value and the original prefix appear, since the 
	 * <CODE>namespaceURI</CODE> and <CODE>localName</CODE> do not change.
	 *
	 * @param prefix The prefix of this node.
	 * @exception DOMException
	 *	 INVALID_CHARACTER_ERR: Raised if the specified prefix contains
	 *							an illegal character.
	 * <br>
	 *	 NO_MODIFICATION_ALLOWED_ERR: Raised if this node is readonly.
	 * <br>
	 *	 NAMESPACE_ERR: Raised if the specified <CODE>prefix</CODE> is 
	 *		 malformed, if the specified prefix is "xml" and the 
	 *		 <CODE>namespaceURI</CODE> of this node is different from 
	 *		 "http://www.w3.org/XML/1998/namespace", if specified prefix is 
	 *		 "xmlns" and the <CODE>namespaceURI</CODE> is neither 
	 *		 <CODE>null</CODE> nor an empty string, or if the 
	 *		 <CODE>localName</CODE> is <CODE>null</CODE>.
	 */
	virtual void
	setPrefix(const XalanDOMString&	prefix) = 0;

	/**
	 * Determine if the document is node-order indexed.
	 *
	 * @return true if the document is indexed, otherwise false.
	 */
	virtual bool
	isIndexed() const = 0;

	/**
	 * Get the node's index.  Valid only if the owner document
	 * reports that the document is node-order indexed.
	 *
	 * @return The index value, or 0 if the node is not indexed.
	 */
	virtual IndexType
	getIndex() const = 0;

	//@}

protected:

	XalanNode(const XalanNode&	theSource);

	XalanNode&
	operator=(const XalanNode&	theSource);

	bool
	operator==(const XalanNode&		theRHS) const;

private:

#if !defined(NDEBUG)
	static size_t	s_instanceCount;
#endif
};



XALAN_CPP_NAMESPACE_END



#endif	// !defined(XALANNODE_HEADER_GUARD_1357924680)
