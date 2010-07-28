
/// @file libym_mvn/dump.cc
/// @brief dump() の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010 Yusuke Matsunaga
/// All rights reserved.


#include "ym_mvn/MvMgr.h"

#include "ym_mvn/MvModule.h"
#include "ym_mvn/MvPort.h"
#include "ym_mvn/MvNode.h"
#include "ym_mvn/MvNet.h"
#include "ym_mvn/MvPin.h"


BEGIN_NAMESPACE_YM_MVN

BEGIN_NONAMESPACE

string
node_name(const MvNode* node)
{
  ostringstream buf;
  buf << "node" << node->id();
  return buf.str();
}

void
dump_port_ref(ostream& s,
	      const MvPortRef* port_ref)
{
  s << node_name(port_ref->node());
  if ( port_ref->has_bitselect() ) {
    s << "[" << port_ref->bitpos() << "]";
  }
  else if ( port_ref->has_partselect() ) {
    s << "[" << port_ref->msb() << ":" << port_ref->lsb() << "]";
  }
}

void
dump_port(ostream& s,
	  const MvPort* port)
{
  s << "." << port->name() << "(";
  ymuint n = port->port_ref_num();
  if ( n == 1 ) {
    const MvPortRef* port_ref = port->port_ref(0);
    dump_port_ref(s, port_ref);
  }
  else if ( n > 1 ) {
    s << "{";
    const char* comma = "";
    for (ymuint i = 0; i < n; ++ i) {
      s << comma;
      comma = ", ";
      const MvPortRef* port_ref = port->port_ref(i);
      dump_port_ref(s, port_ref);
    }
    s << "}";
  }
  s << ")";
}

void
dump_node(ostream& s,
	  const MvNode* node)
{
  switch ( node->type() ) {
  case MvNode::kInput:
    break;

  case MvNode::kOutput:
    {
      const MvInputPin* ipin = node->input(0);
      const MvNet* net = ipin->net();
      const MvOutputPin* src_pin = net->src_pin();
      const MvNode* src_node = src_pin->node();
      s << "  assign " << node_name(node)
	<< " = " << node_name(src_node)
	<< ";" << endl;
    }
    break;

  case MvNode::kDff1:
    {
      s << "  GTECH_FD2 U" << node->id()
	<< " (";

      s << " .Q(" << node_name(node) << ")";

      const char* comma = ", ";

      const MvInputPin* ipin0 = node->input(0);
      const MvNet* net0 = ipin0->net();
      if ( net0 ) {
	const MvOutputPin* src_pin0 = net0->src_pin();
	const MvNode* src_node0 = src_pin0->node();
	s << comma << " .D(" << node_name(src_node0) << ")";
      }

      const MvInputPin* ipin1 = node->input(1);
      const MvNet* net1 = ipin1->net();
      if ( net1 ) {
	const MvOutputPin* src_pin1 = net1->src_pin();
	const MvNode* src_node1 = src_pin1->node();
	s << comma << " .CP(" << node_name(src_node1) << ")";
      }

      const MvInputPin* ipin2 = node->input(2);
      const MvNet* net2 = ipin2->net();
      if ( net2 ) {
	const MvOutputPin* src_pin2 = net2->src_pin();
	const MvNode* src_node2 = src_pin2->node();
	s << comma << " .CD(" << node_name(src_node2) << ")";
      }
      
      s << ");" << endl;
    }
    break;

  case MvNode::kDff2:
    {
      s << "  GTECH_FD2 U" << node->id()
	<< " (";

      s << " .Q(" << node_name(node) << ")";
      
      const char* comma = ", ";

      const MvInputPin* ipin0 = node->input(0);
      const MvNet* net0 = ipin0->net();
      if ( net0 ) {
	const MvOutputPin* src_pin0 = net0->src_pin();
	const MvNode* src_node0 = src_pin0->node();
	s << comma << " .D(" << node_name(src_node0) << ")";
      }

      const MvInputPin* ipin1 = node->input(1);
      const MvNet* net1 = ipin1->net();
      if ( net1 ) {
	const MvOutputPin* src_pin1 = net1->src_pin();
	const MvNode* src_node1 = src_pin1->node();
	s << comma << " .CP(" << node_name(src_node1) << ")";
      }

      const MvInputPin* ipin2 = node->input(2);
      const MvNet* net2 = ipin2->net();
      if ( net2 ) {
	const MvOutputPin* src_pin2 = net2->src_pin();
	const MvNode* src_node2 = src_pin2->node();
	s << comma << " .CD(" << node_name(src_node2) << ")";
      }

      s << ");" << endl;
    }
    break;

  case MvNode::kThrough:
    {
      const MvInputPin* ipin = node->input(0);
      const MvNet* net = ipin->net();
      const MvOutputPin* src_pin = net->src_pin();
      const MvNode* src_node = src_pin->node();
      s << "  assign " << node_name(node)
	<< " = " << node_name(src_node)
	<< ";" << endl;
    }
    break;

  case MvNode::kNot:
    {
      const MvInputPin* ipin = node->input(0);
      const MvNet* net = ipin->net();
      const MvOutputPin* src_pin = net->src_pin();
      const MvNode* src_node = src_pin->node();
      s << "  assign " << node_name(node)
	<< " = ~" << node_name(src_node)
	<< ";" << endl;
    }
    break;

  case MvNode::kAnd:
    {
      const MvInputPin* ipin0 = node->input(0);
      const MvNet* net0 = ipin0->net();
      const MvOutputPin* src_pin0 = net0->src_pin();
      const MvNode* src_node0 = src_pin0->node();

      const MvInputPin* ipin1 = node->input(1);
      const MvNet* net1 = ipin1->net();
      const MvOutputPin* src_pin1 = net1->src_pin();
      const MvNode* src_node1 = src_pin1->node();

      s << "  assign " << node_name(node)
	<< " = " << node_name(src_node0)
	<< " & " << node_name(src_node1)
	<< ";" << endl;
    }
    break;

  case MvNode::kOr:
    {
      const MvInputPin* ipin0 = node->input(0);
      const MvNet* net0 = ipin0->net();
      const MvOutputPin* src_pin0 = net0->src_pin();
      const MvNode* src_node0 = src_pin0->node();

      const MvInputPin* ipin1 = node->input(1);
      const MvNet* net1 = ipin1->net();
      const MvOutputPin* src_pin1 = net1->src_pin();
      const MvNode* src_node1 = src_pin1->node();

      s << "  assign " << node_name(node)
	<< " = " << node_name(src_node0)
	<< " | " << node_name(src_node1)
	<< ";" << endl;
    }
    break;

  case MvNode::kXor:
    {
      const MvInputPin* ipin0 = node->input(0);
      const MvNet* net0 = ipin0->net();
      const MvOutputPin* src_pin0 = net0->src_pin();
      const MvNode* src_node0 = src_pin0->node();

      const MvInputPin* ipin1 = node->input(1);
      const MvNet* net1 = ipin1->net();
      const MvOutputPin* src_pin1 = net1->src_pin();
      const MvNode* src_node1 = src_pin1->node();

      s << "  assign " << node_name(node)
	<< " = " << node_name(src_node0)
	<< " ^ " << node_name(src_node1)
	<< ";" << endl;
    }
    break;

  case MvNode::kRand:
    {
      const MvInputPin* ipin = node->input(0);
      const MvNet* net = ipin->net();
      const MvOutputPin* src_pin = net->src_pin();
      const MvNode* src_node = src_pin->node();
      s << "  assign " << node_name(node)
	<< " = &" << node_name(src_node)
	<< ";" << endl;
    }
    break;

  case MvNode::kRor:
    {
      const MvInputPin* ipin = node->input(0);
      const MvNet* net = ipin->net();
      const MvOutputPin* src_pin = net->src_pin();
      const MvNode* src_node = src_pin->node();
      s << "  assign " << node_name(node)
	<< " = |" << node_name(src_node)
	<< ";" << endl;
    }
    break;

  case MvNode::kRxor:
    {
      const MvInputPin* ipin = node->input(0);
      const MvNet* net = ipin->net();
      const MvOutputPin* src_pin = net->src_pin();
      const MvNode* src_node = src_pin->node();
      s << "  assign " << node_name(node)
	<< " = ^" << node_name(src_node)
	<< ";" << endl;
    }
    break;

  case MvNode::kEq:
    {
      const MvInputPin* ipin0 = node->input(0);
      const MvNet* net0 = ipin0->net();
      const MvOutputPin* src_pin0 = net0->src_pin();
      const MvNode* src_node0 = src_pin0->node();

      const MvInputPin* ipin1 = node->input(1);
      const MvNet* net1 = ipin1->net();
      const MvOutputPin* src_pin1 = net1->src_pin();
      const MvNode* src_node1 = src_pin1->node();

      s << "  assign " << node_name(node)
	<< " = (" << node_name(src_node0)
	<< " == " << node_name(src_node1)
	<< ");" << endl;
    }
    break;

  case MvNode::kLt:
    {
      const MvInputPin* ipin0 = node->input(0);
      const MvNet* net0 = ipin0->net();
      const MvOutputPin* src_pin0 = net0->src_pin();
      const MvNode* src_node0 = src_pin0->node();

      const MvInputPin* ipin1 = node->input(1);
      const MvNet* net1 = ipin1->net();
      const MvOutputPin* src_pin1 = net1->src_pin();
      const MvNode* src_node1 = src_pin1->node();

      s << "  assign " << node_name(node)
	<< " = (" << node_name(src_node0)
	<< " < " << node_name(src_node1)
	<< ");" << endl;
    }
    break;

  case MvNode::kSll:
    break;

  case MvNode::kSrl:
    break;

  case MvNode::kSla:
    break;

  case MvNode::kSra:
    break;

  case MvNode::kAdd:
    {
      const MvInputPin* ipin0 = node->input(0);
      const MvNet* net0 = ipin0->net();
      const MvOutputPin* src_pin0 = net0->src_pin();
      const MvNode* src_node0 = src_pin0->node();

      const MvInputPin* ipin1 = node->input(1);
      const MvNet* net1 = ipin1->net();
      const MvOutputPin* src_pin1 = net1->src_pin();
      const MvNode* src_node1 = src_pin1->node();

      s << "  assign " << node_name(node)
	<< " = " << node_name(src_node0)
	<< " + " << node_name(src_node1)
	<< ";" << endl;
    }
    break;

  case MvNode::kSub:
    {
      const MvInputPin* ipin0 = node->input(0);
      const MvNet* net0 = ipin0->net();
      const MvOutputPin* src_pin0 = net0->src_pin();
      const MvNode* src_node0 = src_pin0->node();

      const MvInputPin* ipin1 = node->input(1);
      const MvNet* net1 = ipin1->net();
      const MvOutputPin* src_pin1 = net1->src_pin();
      const MvNode* src_node1 = src_pin1->node();

      s << "  assign " << node_name(node)
	<< " = " << node_name(src_node0)
	<< " - " << node_name(src_node1)
	<< ";" << endl;
    }
    break;

  case MvNode::kMult:
    {
      const MvInputPin* ipin0 = node->input(0);
      const MvNet* net0 = ipin0->net();
      const MvOutputPin* src_pin0 = net0->src_pin();
      const MvNode* src_node0 = src_pin0->node();

      const MvInputPin* ipin1 = node->input(1);
      const MvNet* net1 = ipin1->net();
      const MvOutputPin* src_pin1 = net1->src_pin();
      const MvNode* src_node1 = src_pin1->node();

      s << "  assign " << node_name(node)
	<< " = " << node_name(src_node0)
	<< " * " << node_name(src_node1)
	<< ";" << endl;
    }
    break;

  case MvNode::kDiv:
    {
      const MvInputPin* ipin0 = node->input(0);
      const MvNet* net0 = ipin0->net();
      const MvOutputPin* src_pin0 = net0->src_pin();
      const MvNode* src_node0 = src_pin0->node();

      const MvInputPin* ipin1 = node->input(1);
      const MvNet* net1 = ipin1->net();
      const MvOutputPin* src_pin1 = net1->src_pin();
      const MvNode* src_node1 = src_pin1->node();

      s << "  assign " << node_name(node)
	<< " = " << node_name(src_node0)
	<< " / " << node_name(src_node1)
	<< ";" << endl;
    }
    break;

  case MvNode::kMod:
    {
      const MvInputPin* ipin0 = node->input(0);
      const MvNet* net0 = ipin0->net();
      const MvOutputPin* src_pin0 = net0->src_pin();
      const MvNode* src_node0 = src_pin0->node();

      const MvInputPin* ipin1 = node->input(1);
      const MvNet* net1 = ipin1->net();
      const MvOutputPin* src_pin1 = net1->src_pin();
      const MvNode* src_node1 = src_pin1->node();

      s << "  assign " << node_name(node)
	<< " = " << node_name(src_node0)
	<< " % " << node_name(src_node1)
	<< ";" << endl;
    }
    break;

  case MvNode::kPow:
    {
      const MvInputPin* ipin0 = node->input(0);
      const MvNet* net0 = ipin0->net();
      const MvOutputPin* src_pin0 = net0->src_pin();
      const MvNode* src_node0 = src_pin0->node();

      const MvInputPin* ipin1 = node->input(1);
      const MvNet* net1 = ipin1->net();
      const MvOutputPin* src_pin1 = net1->src_pin();
      const MvNode* src_node1 = src_pin1->node();

      s << "  assign " << node_name(node)
	<< " = " << node_name(src_node0)
	<< " ** " << node_name(src_node1)
	<< ";" << endl;
    }
    break;

  case MvNode::kIte:
    {
      const MvInputPin* ipin0 = node->input(0);
      const MvNet* net0 = ipin0->net();
      const MvOutputPin* src_pin0 = net0->src_pin();
      const MvNode* src_node0 = src_pin0->node();

      const MvInputPin* ipin1 = node->input(1);
      const MvNet* net1 = ipin1->net();
      const MvOutputPin* src_pin1 = net1->src_pin();
      const MvNode* src_node1 = src_pin1->node();

      const MvInputPin* ipin2 = node->input(2);
      const MvNet* net2 = ipin2->net();
      const MvOutputPin* src_pin2 = net2->src_pin();
      const MvNode* src_node2 = src_pin2->node();

      s << "  assign " << node_name(node)
	<< " = " << node_name(src_node0)
	<< " ? " << node_name(src_node1)
	<< " : " << node_name(src_node2)
	<< ";" << endl;
    }
    break;

  case MvNode::kConcat:
    {
      s << "  assign " << node_name(node)
	<< " = {";
      const char* comma = "";
      ymuint ni = node->input_num();
      for (ymuint i = 0; i < ni; ++ i) {
	const MvInputPin* ipin = node->input(i);
	const MvNet* net = ipin->net();
	const MvOutputPin* opin = net->src_pin();
	const MvNode* src_node = opin->node();
	s << comma << node_name(src_node);
	comma = ", ";
      }
      s << "};" << endl;
    }
    break;

  case MvNode::kConstBitSelect:
    {
      const MvInputPin* ipin = node->input(0);
      const MvNet* net = ipin->net();
      const MvOutputPin* src_pin = net->src_pin();
      const MvNode* src_node = src_pin->node();
      s << "  assign " << node_name(node)
	<< " = " << node_name(src_node)
	<< "[" << node->bitpos() << "];" << endl;
    }
    break;

  case MvNode::kConstPartSelect:
    {
      const MvInputPin* ipin = node->input(0);
      const MvNet* net = ipin->net();
      const MvOutputPin* src_pin = net->src_pin();
      const MvNode* src_node = src_pin->node();
      s << "  assign " << node_name(node)
	<< " = " << node_name(src_node)
	<< "[" << node->msb()
	<< ":" << node->lsb()
	<< "];" << endl;
    }
    break;

  case MvNode::kBitSelect:
    {
      const MvInputPin* ipin = node->input(0);
      const MvNet* net = ipin->net();
      const MvOutputPin* src_pin = net->src_pin();
      const MvNode* src_node = src_pin->node();

      const MvInputPin* ipin1 = node->input(1);
      const MvNet* net1 = ipin1->net();
      const MvOutputPin* src_pin1 = net1->src_pin();
      const MvNode* src_node1 = src_pin1->node();
      
      s << "  assign " << node_name(node)
	<< " = " << node_name(src_node)
	<< "[" << node_name(src_node1) << "];" << endl;
    }
    break;

  case MvNode::kPartSelect:
    {
      const MvInputPin* ipin = node->input(0);
      const MvNet* net = ipin->net();
      const MvOutputPin* src_pin = net->src_pin();
      const MvNode* src_node = src_pin->node();

      const MvInputPin* ipin1 = node->input(1);
      const MvNet* net1 = ipin1->net();
      const MvOutputPin* src_pin1 = net1->src_pin();
      const MvNode* src_node1 = src_pin1->node();

      const MvInputPin* ipin2 = node->input(2);
      const MvNet* net2 = ipin2->net();
      const MvOutputPin* src_pin2 = net2->src_pin();
      const MvNode* src_node2 = src_pin2->node();
      
      s << "  assign " << node_name(node)
	<< " = " << node_name(src_node)
	<< "[" << node_name(src_node1)
	<< ":" << node_name(src_node2)
	<< "];" << endl;
    }
    break;

  case MvNode::kCombUdp:
    {
      s << "  MVN_UDP" << node->id()
	<< " U" << node->id()
	<< " ("
	<< " " << node_name(node);
      ymuint ni = node->input_num();
      for (ymuint i = 0; i < ni; ++ i) {
	const MvInputPin* ipin = node->input(i);
	const MvNet* net = ipin->net();
	const MvOutputPin* src_pin = net->src_pin();
	const MvNode* src_node = src_pin->node();
	s << ", " << node_name(src_node);
      }
      s << ");" << endl;
    }
    break;

  case MvNode::kSeqUdp:
    break;

  case MvNode::kConst:
    break;

  default:
    break;
  }
}

void
dump_module(ostream& s,
	    const MvModule* module,
	    const MvMgr& mgr)
{
  s << "module " << module->name() << "(";

  ymuint np = module->port_num();
  const char* comma = "";
  for (ymuint i = 0; i < np; ++ i) {
    s << comma;
    comma = ", ";
    const MvPort* port = module->port(i);
    dump_port(s, port);
  }

  s << ");" << endl;

  ymuint ni = module->input_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const MvNode* node = module->input(i);
    ymuint bw = node->output(0)->bit_width();
    assert_cond( bw > 0, __FILE__, __LINE__);
    if ( bw == 1 ) {
      s << "  input ";
    }
    else {
      s << "  input [" << bw - 1 << ":0] ";
    }
    s << node_name(node) << ";" << endl;
  }

  ymuint no = module->output_num();
  for (ymuint i = 0; i < no; ++ i) {
    const MvNode* node = module->output(i);
    ymuint bw = node->input(0)->bit_width();
    assert_cond( bw > 0, __FILE__, __LINE__);
    if ( bw == 1 ) {
      s << "  output ";
    }
    else {
      s << "  output [" << bw - 1 << ":0] ";
    }
    s << node_name(node) << ";" << endl;
  }
  s << endl;
    
  const list<MvNode*>& node_list = module->node_list();
  for (list<MvNode*>::const_iterator p = node_list.begin();
       p != node_list.end(); ++ p) {
    MvNode* node = *p;
    assert_cond( node->output_num() == 1, __FILE__, __LINE__);
    ymuint bw = node->output(0)->bit_width();
    s << "  wire ";
    if ( bw > 1 ) {
      s << "[" << bw - 1 << ":" << "0]";
    }
    s << " " << node_name(node) << ";" << endl;
  }
  s << endl;

  for (ymuint i = 0; i < no; ++ i) {
    const MvNode* node = module->output(i);
    const MvInputPin* ipin = node->input(0);
    const MvNet* net = ipin->net();
    const MvOutputPin* src_pin = net->src_pin();
    const MvNode* src_node = src_pin->node();
    s << "  assign " << node_name(node)
      << " = " << node_name(src_node)
      << ";" << endl;
  }
  for (list<MvNode*>::const_iterator p = node_list.begin();
       p != node_list.end(); ++ p) {
    MvNode* node = *p;
    dump_node(s, node);
  }

  s << "endmodule" << endl
    << endl;
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// グローバル関数
//////////////////////////////////////////////////////////////////////

// @brief 内容を Verilog-HDL 形式で出力する
// @param[in] s 出力先のストリーム
// @param[in] mgr MvMgr
void
dump_verilog(ostream& s,
	     const MvMgr& mgr)
{
  ymuint n = mgr.max_module_id();
  for (ymuint i = 0; i < n; ++ i) {
    const MvModule* module = mgr.module(i);
    if ( module == NULL ) continue;

    dump_module(s, module, mgr);
  }
}

END_NAMESPACE_YM_MVN