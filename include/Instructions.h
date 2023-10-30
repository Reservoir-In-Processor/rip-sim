#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H
#include "CSR.h"
#include "Exceptions.h"
#include "InstructionTypes.h"
#include "Memory.h"
#include "Registers.h"
#include <cassert>
#include <iomanip>
#include <iostream>
#include <optional>
#include <vector>

namespace {

static const std::pair<int, std::string>
parseOffsetReg(const std::string &input) {
  std::string numStr, identifier;

  size_t i = 0;
  while (i < input.size() && (std::isdigit(input[i]) || input[i] == '-')) {
    numStr.push_back(input[i]);
    i++;
  }

  while (i < input.size() && (std::isalnum(input[i]) || input[i] == '_')) {
    identifier.push_back(input[i]);
    i++;
  }

  int num;
  try {
    num = std::stoi(numStr);
  } catch (const std::exception &e) {
    throw std::runtime_error("Failed to parse number from string.");
  }

  return {num, identifier};
}

static const std::optional<std::bitset<5>> findReg(const std::string &RegStr) {
  if (auto RI = GPRegMap.find(RegStr); RI != GPRegMap.end())
    return RI->second;
  assert(false && "invalid register name");
  return std::nullopt;
}

} // namespace
class Instruction {
  unsigned Val;

public:
  void setVal(unsigned V) { Val = V; }

  // TODO: make this private.
  const unsigned getVal() { return Val; }

  const inline bool hasRd() {
    return !(STypeKinds.count(getMnemo()) || BTypeKinds.count(getMnemo()));
  }

  const inline unsigned getRd() {
    if (!hasRd())
      assert(false && "This inst don't have rd!");
    return (Val & 0x00000f80) >> 7;
  }

  const inline bool hasRs1() {
    return !(UTypeKinds.count(getMnemo()) || JTypeKinds.count(getMnemo()));
  }

  const inline unsigned getRs1() {
    if (!hasRs1())
      assert(false && "This inst don't have rs1!");
    return (Val & 0x000f8000) >> 15;
  }

  const inline bool hasRs2() {
    return !(ITypeKinds.count(getMnemo()) || UTypeKinds.count(getMnemo()) ||
             JTypeKinds.count(getMnemo()));
  }

  const inline unsigned getRs2() {
    if (!hasRs2())
      assert(false && "This inst don't have rs2!");
    return (Val & 0x01f00000) >> 20;
  }

  const inline unsigned getIImm() {
    if (!ITypeKinds.count(getMnemo()))
      assert(false && "This isn't expected to be called on not I-inst!");
    return (Val & 0xfff00000) >> 20;
  }

  const inline unsigned getSImm() {
    if (!STypeKinds.count(getMnemo()))
      assert(false && "This isn't expected to be called on not S-inst!");
    return (Val & 0xfe000000) >> 20 | ((Val >> 7) & 0x1f);
  }

  const inline unsigned getJImm() {
    if (!JTypeKinds.count(getMnemo()))
      assert(false && "This isn't expected to be called on not J-inst!");
    return ((Val & 0x80000000) >> 11) | (Val & 0xff000) | ((Val >> 9) & 0x800) |
           ((Val >> 20) & 0x7fe);
  }

  const inline unsigned getBImm() {
    if (!BTypeKinds.count(getMnemo()))
      assert(false && "This isn't expected to be called on not B-inst!");
    return (Val & 0x80000000) >> 19 | ((Val & 0x80) << 4) |
           ((Val >> 20) & 0x7e0) | ((Val >> 7) & 0x1e);
  }

  const inline unsigned getUImm() {
    if (!UTypeKinds.count(getMnemo()))
      assert(false && "This isn't expected to be called on not U-inst!");
    return (Val & 0xfffff000) >> 12;
  }

  virtual const std::string &getMnemo() = 0;

  void emitBinary(std::ostream &os) {
    os.write(reinterpret_cast<char *>(&Val), 4);
  }
  virtual void pprint(std::ostream &) = 0;
  virtual void mprint(std::ostream &) = 0;
  virtual std::optional<Exception> exec(Address &, GPRegisters &, Memory &,
                                        CSRs &, ModeKind &) = 0;
  virtual ~Instruction() {}
};

namespace {
template <std::size_t N> int signExtend(const std::bitset<N> &bs) {
  if (bs[N - 1]) {
    std::bitset<sizeof(RegVal) * 8> extended = ~0;
    for (std::size_t i = 0; i < N; ++i) {
      extended[i] = bs[i];
    }
    return static_cast<int>(extended.to_ulong());
  } else {
    return static_cast<int>(bs.to_ulong());
  }
}

} // namespace

class IInstruction : public Instruction {
private:
  const ISBType &IT;

  std::bitset<5> Rd, Rs1;
  std::bitset<12> Imm;

public:
  /// Encoding
  IInstruction(const ISBType &IT, const std::vector<std::string> &Toks)
      : IT(IT) {
    Rd = *findReg(Toks[1]);

    // handle offset for loads
    if (Toks.size() == 3) {
      assert((IT.getMnemo() == "lb" || IT.getMnemo() == "lh" ||
              IT.getMnemo() == "lw" || IT.getMnemo() == "lbu" ||
              IT.getMnemo() == "lhu") &&
             "invarid offset(reg) notation except loads");
      // op rd, offset(rs1)
      auto OffReg = parseOffsetReg(Toks[2]);
      Rs1 = std::bitset<5>(OffReg.second);
      Imm = OffReg.first;
    } else {
      // op rd,rs1,imm
      Rs1 = *findReg(Toks[2]);
      Imm = stoi(Toks[3]);
    }

    // srai immediate
    if (Toks[0] == "srai")
      Imm |= 1 << 10;

    setVal((Imm.to_ulong() << 20) | (Rs1.to_ulong() << 15) |
           (IT.getFunct3().to_ulong() << 12) | (Rd.to_ulong() << 7) |
           IT.getOpcode().to_ulong());
  }

  IInstruction(const ISBType &IT, const unsigned Rd, const unsigned Rs1,
               const unsigned Imm)
      : IT(IT), Rd(Rd), Rs1(Rs1), Imm(Imm) {
    // FIXME: rename member as _XX?
    setVal((this->Imm.to_ulong() << 20) | (this->Rs1.to_ulong() << 15) |
           (IT.getFunct3().to_ulong() << 12) | (this->Rd.to_ulong() << 7) |
           IT.getOpcode().to_ulong());
  }

  const std::string &getMnemo() override { return IT.getMnemo(); }

  void mprint(std::ostream &os) override {
    if (IT.getMnemo() == "lb" || IT.getMnemo() == "lh" ||
        IT.getMnemo() == "lw" || IT.getMnemo() == "lbu" ||
        IT.getMnemo() == "lhu")
      os << IT.getMnemo() << " " << ABI[Rd.to_ulong()] << " "
         << "(" << std::dec << signExtend(Imm) << ")" << ABI[Rs1.to_ulong()];
    else if ((int)IT.getMnemo().rfind("csr") != -1 &&
             (int)IT.getMnemo().rfind("i") == -1) {
      CSRAddress CSRAddr = (unsigned)signExtend(Imm);
      os << IT.getMnemo() << " " << ABI[Rd.to_ulong()] << " "
         << " " << std::dec
         << (CSRNames.count(CSRAddr) ? CSRNames.find(CSRAddr)->second
                                     : std::to_string(CSRAddr))
         << " " << ABI[Rs1.to_ulong()];
    } else if ((int)IT.getMnemo().rfind("csr") != -1 &&
               (int)IT.getMnemo().rfind("i") != -1) {
      CSRAddress CSRAddr = (unsigned)signExtend(Imm);
      os << IT.getMnemo() << " " << ABI[Rd.to_ulong()] << " "
         << " " << std::dec
         << (CSRNames.count(CSRAddr) ? CSRNames.find(CSRAddr)->second
                                     : std::to_string(CSRAddr))
         << " " << Rs1.to_ulong();
    } else if (IT.getMnemo() == "ecall" || IT.getMnemo() == "ebreak" ||
               (int)IT.getMnemo().rfind("ret") != -1) {
      os << IT.getMnemo();
    } else
      os << IT.getMnemo() << " " << ABI[Rd.to_ulong()] << " "
         << ABI[Rs1.to_ulong()] << " " << std::dec << signExtend(Imm);
  }

  void pprint(std::ostream &os) override {
    if (IT.getMnemo() == "lb" || IT.getMnemo() == "lh" ||
        IT.getMnemo() == "lw" || IT.getMnemo() == "lbu" ||
        IT.getMnemo() == "lhu")
      os << IT.getMnemo() << " " << ABI[Rd.to_ulong()] << " "
         << "(" << std::dec << signExtend(Imm) << ")" << ABI[Rs1.to_ulong()];
    else
      os << IT.getMnemo() << " " << ABI[Rd.to_ulong()] << " "
         << ABI[Rs1.to_ulong()] << " " << std::dec << signExtend(Imm);
    os << " := ";

    unsigned V = getVal();
    for (int i = 0; i < 32; ++i) {
      if (i == 12 || i == 17 || i == 20 || i == 25)
        os << ' ';
      os << (V >> (31 - i) & 1);
    }

    os << "(BIN) = ";
    for (unsigned long i = 0; i < sizeof(V); ++i) {
      unsigned char byte = (V >> (i * 8)) & 0xFF;
      std::cerr << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<int>(byte) << ' ';
    }
    os << "(HEX LE)";
    os << "\n";
  }
  std::optional<Exception> exec(Address &P, GPRegisters &GPRegs, Memory &M,
                                CSRs &CSRs, ModeKind &Mode) override;
};

class RInstruction : public Instruction {
private:
  const RType &RT;

  std::bitset<5> Rd, Rs1, Rs2;

public:
  /// Encoding
  RInstruction(const RType &RT, const std::vector<std::string> &Toks) : RT(RT) {
    Rd = *findReg(Toks[1]);
    Rs1 = *findReg(Toks[2]);
    Rs2 = *findReg(Toks[3]);
    setVal((RT.getFunct7().to_ulong() << 25) | (Rs2.to_ulong() << 20) |
           (Rs1.to_ulong() << 15) | (RT.getFunct3().to_ulong() << 12) |
           (Rd.to_ulong() << 7) | RT.getOpcode().to_ulong());
  }

  RInstruction(const RType &RT, const unsigned Rd, const unsigned Rs1,
               const unsigned Rs2)
      : RT(RT), Rd(Rd), Rs1(Rs1), Rs2(Rs2) {
    // FIXME: rename member as _XX?
    setVal((RT.getFunct7().to_ulong() << 25) | (this->Rs2.to_ulong() << 20) |
           (this->Rs1.to_ulong() << 15) | (RT.getFunct3().to_ulong() << 12) |
           (this->Rd.to_ulong() << 7) | RT.getOpcode().to_ulong());
  }

  const std::string &getMnemo() override { return RT.getMnemo(); }

  void mprint(std::ostream &os) override {
    os << RT.getMnemo() << " " << ABI[Rd.to_ulong()] << " "
       << ABI[Rs1.to_ulong()] << " " << ABI[Rs2.to_ulong()];
  }

  void pprint(std::ostream &os) override {
    os << RT.getMnemo() << " " << ABI[Rd.to_ulong()] << " "
       << ABI[Rs1.to_ulong()] << " " << ABI[Rs2.to_ulong()];
    os << " := ";

    unsigned V = getVal();
    for (int i = 0; i < 32; ++i) {
      if (i == 7 || i == 12 || i == 17 || i == 20 || i == 25)
        os << ' ';
      os << (V >> (31 - i) & 1);
    }

    os << "(BIN) = ";
    for (unsigned long i = 0; i < sizeof(V); ++i) {
      unsigned char byte = (V >> (i * 8)) & 0xFF;
      os << std::hex << std::setw(2) << std::setfill('0')
         << static_cast<int>(byte) << ' ';
    }
    os << "(HEX LE)";
    os << "\n";
  }

  std::optional<Exception> exec(Address &, GPRegisters &, Memory &, CSRs &CSRs,
                                ModeKind &Mode) override;
};

class UInstruction : public Instruction {
private:
  const UJType &UT;

  std::bitset<5> Rd;
  std::bitset<20> Imm;

public:
  /// This is expected to be used on asm.
  UInstruction(const UJType &UT, const std::vector<std::string> &Toks)
      : UT(UT) {
    Rd = *findReg(Toks[1]);
    Imm = stoi(Toks[2]);
    setVal((Imm.to_ulong() << 12) | (Rd.to_ulong() << 7) |
           UT.getOpcode().to_ulong());
  }

  UInstruction(const UJType &UT, const unsigned Rd, const unsigned Imm)
      : UT(UT), Rd(Rd), Imm(Imm) {
    // FIXME: rename member as _XX?
    setVal((this->Imm.to_ulong() << 12) | (this->Rd.to_ulong() << 7) |
           UT.getOpcode().to_ulong());
  }
  const std::string &getMnemo() override { return UT.getMnemo(); }

  void mprint(std::ostream &os) override {
    os << UT.getMnemo() << " " << ABI[Rd.to_ulong()] << " " << std::dec
       << signExtend(Imm);
  }

  void pprint(std::ostream &os) override {
    os << UT.getMnemo() << " " << ABI[Rd.to_ulong()] << " " << std::dec
       << signExtend(Imm);
    os << " := ";

    unsigned V = getVal();
    for (int i = 0; i < 32; ++i) {
      if (i == 20 || i == 25)
        os << ' ';
      os << (V >> (31 - i) & 1);
    }

    os << "(BIN) = ";
    for (unsigned long i = 0; i < sizeof(V); ++i) {
      unsigned char byte = (V >> (i * 8)) & 0xFF;
      os << std::hex << std::setw(2) << std::setfill('0')
         << static_cast<int>(byte) << ' ';
    }
    os << "(HEX LE)";
    os << "\n";
  }
  std::optional<Exception> exec(Address &, GPRegisters &, Memory &, CSRs &CSRs,
                                ModeKind &Mode) override;
};

class JInstruction : public Instruction {
private:
  const UJType &JT;

  std::bitset<5> Rd;
  std::bitset<21> Imm;

public:
  /// Encoding
  JInstruction(const UJType &JT, const std::vector<std::string> &Toks)
      : JT(JT) {
    unsigned M0 = 0b100000000000000000000;
    unsigned M1 = 0b000000000011111111110;
    unsigned M2 = 0b000000000100000000000;
    unsigned M3 = 0b011111111000000000000;
    Rd = *findReg(Toks[1]);
    Imm = stoi(Toks[2]);
    unsigned ImmU = Imm.to_ulong();
    setVal((((ImmU & M0) >> 20) << 31) | (((ImmU & M1) >> 1) << 21) |
           (((ImmU & M2) >> 11) << 20) | (((ImmU & M3) >> 12) << 12) |
           (Rd.to_ulong() << 7) | JT.getOpcode().to_ulong());
  }

  JInstruction(const UJType &JT, const unsigned Rd, const unsigned Imm)
      : JT(JT), Rd(Rd), Imm(Imm) {
    unsigned M0 = 0b100000000000000000000;
    unsigned M1 = 0b000000000011111111110;
    unsigned M2 = 0b000000000100000000000;
    unsigned M3 = 0b011111111000000000000;
    setVal((((Imm & M0) >> 20) << 31) | (((Imm & M1) >> 1) << 21) |
           (((Imm & M2) >> 11) << 20) | (((Imm & M3) >> 12) << 12) | (Rd << 7) |
           JT.getOpcode().to_ulong());
  }
  const std::string &getMnemo() override { return JT.getMnemo(); }

  void mprint(std::ostream &os) override {
    os << JT.getMnemo() << " " << ABI[Rd.to_ulong()] << " " << std::dec
       << signExtend(Imm);
  }

  void pprint(std::ostream &os) override {
    os << JT.getMnemo() << " " << ABI[Rd.to_ulong()] << " " << std::dec
       << signExtend(Imm);
    os << " :=";

    unsigned V = getVal();
    for (int i = 0; i < 32; ++i) {
      if (i == 20 || i == 25)
        os << ' ';
      os << (V >> (31 - i) & 1);
    }
    os << "(BIN) = ";
    for (unsigned long i = 0; i < sizeof(V); ++i) {
      unsigned char byte = (V >> (i * 8)) & 0xFF;
      os << std::hex << std::setw(2) << std::setfill('0')
         << static_cast<int>(byte) << ' ';
    }
    os << "(HEX LE)";
    os << "\n";
  }
  std::optional<Exception> exec(Address &, GPRegisters &, Memory &, CSRs &CSRs,
                                ModeKind &Mode) override;
};

class SInstruction : public Instruction {
private:
  const ISBType &ST;

  std::bitset<5> Rs1, Rs2;
  std::bitset<12> Imm;

public:
  /// Encoding.
  SInstruction(const ISBType &ST, const std::vector<std::string> &Toks)
      : ST(ST) {

    // sb/h/w rs2,offset(rs1)
    Rs2 = *findReg(Toks[1]);
    auto OffReg = parseOffsetReg(Toks[2]);
    Rs1 = std::bitset<5>(OffReg.second);
    Imm = OffReg.first;
    unsigned M0 = 0b111111100000;
    unsigned M1 = 0b000000011111;
    unsigned ImmU = Imm.to_ulong();
    setVal((((ImmU & M0) >> 5) << 25) | (Rs2.to_ulong() << 20) |
           (Rs1.to_ulong() << 15) | (ST.getFunct3().to_ulong() << 12) |
           ((ImmU & M1) << 7) | ST.getOpcode().to_ulong());
  }

  SInstruction(const ISBType &ST, const unsigned Rs1, const unsigned Rs2,
               const unsigned Imm)
      : ST(ST), Rs1(Rs1), Rs2(Rs2), Imm(Imm) {
    // FIXME: rename member as _XX?
    unsigned M0 = 0b111111100000;
    unsigned M1 = 0b000000011111;
    setVal((((Imm & M0) >> 5) << 25) | (this->Rs2.to_ulong() << 20) |
           (this->Rs1.to_ulong() << 15) | (ST.getFunct3().to_ulong() << 12) |
           ((Imm & M1) << 7) | ST.getOpcode().to_ulong());
  }

  const std::string &getMnemo() override { return ST.getMnemo(); }

  void mprint(std::ostream &os) override {
    os << ST.getMnemo() << " " << ABI[Rs2.to_ulong()] << " "
       << "(" << std::dec << signExtend(Imm) << ")" << ABI[Rs1.to_ulong()];
  }

  void pprint(std::ostream &os) override {
    os << ST.getMnemo() << " " << ABI[Rs2.to_ulong()] << " "
       << "(" << std::dec << signExtend(Imm) << ")" << ABI[Rs1.to_ulong()];
    os << " :=";

    unsigned V = getVal();
    for (int i = 0; i < 32; ++i) {
      if (i == 7 || i == 12 || i == 17 || i == 20 || i == 25)
        os << ' ';
      os << (V >> (31 - i) & 1);
    }

    os << "(BIN) = ";
    for (unsigned long i = 0; i < sizeof(V); ++i) {
      unsigned char byte = (V >> (i * 8)) & 0xFF;
      os << std::hex << std::setw(2) << std::setfill('0')
         << static_cast<int>(byte) << ' ';
    }
    os << "(HEX LE)";
    os << "\n";
  }
  std::optional<Exception> exec(Address &, GPRegisters &, Memory &, CSRs &CSRs,
                                ModeKind &Mode) override;
};

class BInstruction : public Instruction {
private:
  const ISBType &BT;

  std::bitset<5> Rs1, Rs2;
  std::bitset<13> Imm;

public:
  /// This is expected to be used on asm.
  BInstruction(const ISBType &BT, const std::vector<std::string> &Toks)
      : BT(BT) {

    Rs1 = *findReg(Toks[1]);
    // TODO: handle label branch
    Rs2 = *findReg(Toks[2]);

    std::bitset<13> Imm;
    unsigned M0 = 0b1000000000000;
    unsigned M1 = 0b0011111100000;
    unsigned M2 = 0b0000000011110;
    unsigned M3 = 0b0100000000000;
    Imm = stoi(Toks[3]);
    unsigned ImmU = Imm.to_ulong();
    setVal((((ImmU & M0) >> 12) << 31) | (((ImmU & M1) >> 5) << 25) |
           (Rs2.to_ulong() << 20) | (Rs1.to_ulong() << 15) |
           (BT.getFunct3().to_ulong() << 12) | (((ImmU & M2) >> 1) << 8) |
           (((ImmU & M3) >> 11) << 7) | BT.getOpcode().to_ulong());
  }

  BInstruction(const ISBType &BT, const unsigned Rs1, const unsigned Rs2,
               const unsigned Imm)
      : BT(BT), Rs1(Rs1), Rs2(Rs2), Imm(Imm) {
    // FIXME: rename member as _XX?
    unsigned M0 = 0b1000000000000;
    unsigned M1 = 0b0011111100000;
    unsigned M2 = 0b0000000011110;
    unsigned M3 = 0b0100000000000;

    setVal((((Imm & M0) >> 12) << 31) | (((Imm & M1) >> 5) << 25) |
           (this->Rs2.to_ulong() << 20) | (this->Rs1.to_ulong() << 15) |
           (BT.getFunct3().to_ulong() << 12) | (((Imm & M2) >> 1) << 8) |
           (((Imm & M3) >> 11) << 7) | BT.getOpcode().to_ulong());
  }

  const std::string &getMnemo() override { return BT.getMnemo(); }

  void mprint(std::ostream &os) override {
    os << BT.getMnemo() << " " << ABI[Rs1.to_ulong()] << " "
       << ABI[Rs2.to_ulong()] << std::dec << signExtend(Imm);
  }

  void pprint(std::ostream &os) override {
    os << BT.getMnemo() << " " << ABI[Rs1.to_ulong()] << " "
       << ABI[Rs2.to_ulong()] << std::dec << signExtend(Imm);
    os << " :=";
    unsigned V = getVal();
    for (int i = 0; i < 32; ++i) {
      if (i == 7 || i == 12 || i == 17 || i == 20 || i == 25)
        os << ' ';
      os << (V >> (31 - i) & 1);
    }
    os << "(BIN) = ";
    for (unsigned long i = 0; i < sizeof(V); ++i) {
      unsigned char byte = (V >> (i * 8)) & 0xFF;
      os << std::hex << std::setw(2) << std::setfill('0')
         << static_cast<int>(byte) << ' ';
    }
    os << "(HEX LE)";
    os << "\n";
  }
  std::optional<Exception> exec(Address &, GPRegisters &, Memory &, CSRs &CSRs,
                                ModeKind &Mode) override;
};

#endif
