// Copyright (c) 2016, Ben Noordhuis <info@bnoordhuis.nl>
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#include "node.h"
#include "v8.h"

namespace {

inline void Execute(const v8::FunctionCallbackInfo<v8::Value>& info) {
  auto current_context = info.GetIsolate()->GetCurrentContext();
  v8::Local<v8::String> source_string;
  if (!info[0]->ToString(current_context).ToLocal(&source_string)) return;
  auto empty_string = v8::String::Empty(info.GetIsolate());
  v8::ScriptOrigin script_origin(empty_string);
  v8::ScriptCompiler::Source source(source_string, script_origin);
  v8::Local<v8::UnboundScript> unbound_script;
  if (!v8::ScriptCompiler::CompileUnboundScript(info.GetIsolate(), &source)
           .ToLocal(&unbound_script)) {
    return;
  }
  v8::Local<v8::Context> new_context;
  auto second_arg = info[1];
  if (second_arg->IsObject()) {
    new_context = second_arg.As<v8::Object>()->CreationContext();
  } else {
    new_context = v8::Context::New(info.GetIsolate());
    if (new_context.IsEmpty()) return;
    new_context->SetSecurityToken(current_context->GetSecurityToken());
  }
  v8::Context::Scope context_scope(new_context);
  auto script = unbound_script->BindToCurrentContext();
  v8::Local<v8::Value> result;
  if (!script->Run(new_context).ToLocal(&result)) return;
  info.GetReturnValue().Set(result);
}

inline void Initialize(v8::Local<v8::Object> binding) {
  auto isolate = v8::Isolate::GetCurrent();
  auto context = isolate->GetCurrentContext();
  binding->Set(context, v8::String::NewFromUtf8(isolate, "execute"),
               v8::FunctionTemplate::New(isolate, Execute)
                   ->GetFunction(context)
                   .ToLocalChecked());
}

NODE_MODULE(binding, Initialize)

}  // anonymous namespace
